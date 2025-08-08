#include "./RenderListBuilder.hpp"
#include "../web_content.hpp"
#include "../hierarchy.hpp"
#include "../render_queue.hpp"
#include "../transform.hpp"
#include <client/scroll/scrollable_area.hpp>

namespace builtin_scene::layers
{
  RenderListBuilder::RenderListBuilder(const LayerRenderConfig &config)
      : config_(config)
      , isDirty_(true)
  {
  }

  void RenderListBuilder::buildLayerTree(ecs::SystemContext &context)
  {
    clear();

    // Query all entities with WebContent components
    auto webContentEntities = context.queryEntitiesWithComponent<WebContent>();

    // First pass: create layer nodes for all entities
    for (const auto &[entityId, webContent] : webContentEntities)
    {
      auto layerNode = createLayerNode(entityId, context);
      layerNodeMap_[entityId] = layerNode;
      lastSeenLayerIndex_[entityId] = webContent->layer();
    }

    // Second pass: establish parent-child relationships
    for (const auto &[entityId, layerNode] : layerNodeMap_)
    {
      auto parent = findParentLayer(entityId, context);
      if (parent)
      {
        layerNode->parent = parent;
        parent->children.push_back(layerNode);
      }
      else
      {
        rootLayers_.push_back(layerNode);
      }
    }

    // Third pass: sort children and update properties
    for (const auto &[entityId, layerNode] : layerNodeMap_)
    {
      updateLayerNodeProperties(layerNode, context);
      sortLayerChildren(layerNode);
    }

    // Sort root layers by rendering order
    std::sort(rootLayers_.begin(), rootLayers_.end(), [](const std::shared_ptr<LayerNode> &a, const std::shared_ptr<LayerNode> &b)
              {
                if (a->layerIndex != b->layerIndex)
                  return a->layerIndex < b->layerIndex;
                if (a->zIndex != b->zIndex)
                  return a->zIndex < b->zIndex;
                return a->translateZ < b->translateZ; });

    isDirty_ = false;
  }

  std::vector<RenderCommand> RenderListBuilder::generateRenderCommands()
  {
    std::vector<RenderCommand> commands;

    // Traverse all root layers in order
    for (const auto &rootLayer : rootLayers_)
    {
      traverseLayer(rootLayer, commands);
    }

    return commands;
  }

  std::shared_ptr<LayerNode> RenderListBuilder::getLayerNode(ecs::EntityId entityId) const
  {
    auto it = layerNodeMap_.find(entityId);
    return (it != layerNodeMap_.end()) ? it->second : nullptr;
  }

  bool RenderListBuilder::needsRebuild(ecs::SystemContext &context) const
  {
    if (isDirty_)
      return true;

    // Check if any WebContent entities have changed layer index
    auto webContentEntities = context.queryEntitiesWithComponent<WebContent>();

    for (const auto &[entityId, webContent] : webContentEntities)
    {
      auto lastSeenIt = lastSeenLayerIndex_.find(entityId);
      if (lastSeenIt == lastSeenLayerIndex_.end() ||
          lastSeenIt->second != webContent->layer())
      {
        return true;
      }
    }

    // Check if number of entities changed
    return webContentEntities.size() != layerNodeMap_.size();
  }

  void RenderListBuilder::clear()
  {
    rootLayers_.clear();
    layerNodeMap_.clear();
    isDirty_ = true;
  }

  std::shared_ptr<LayerNode> RenderListBuilder::createLayerNode(ecs::EntityId entityId, ecs::SystemContext &context)
  {
    auto webContent = context.getComponent<WebContent>(entityId);
    if (!webContent)
      return nullptr;

    auto layerNode = std::make_shared<LayerNode>(entityId, webContent->layer());

    // Initialize basic properties
    updateLayerNodeProperties(layerNode, context);

    return layerNode;
  }

  std::shared_ptr<LayerNode> RenderListBuilder::findParentLayer(ecs::EntityId entityId, ecs::SystemContext &context)
  {
    // Check hierarchy component for parent relationship
    auto hierarchyChildren = context.getComponent<hierarchy::Children>(entityId);
    if (!hierarchyChildren)
      return nullptr;

    // Look for parent entity in the hierarchy
    auto parentId = hierarchyChildren->parent();
    if (parentId == ecs::EntityId::Invalid())
      return nullptr;

    // Check if parent has WebContent and is a scrollable container
    auto parentWebContent = context.getComponent<WebContent>(parentId);
    if (!parentWebContent)
      return nullptr;

    // Only establish parent relationship if parent layer index is less than child
    auto childWebContent = context.getComponent<WebContent>(entityId);
    if (!childWebContent || parentWebContent->layer() >= childWebContent->layer())
      return nullptr;

    // Look for the parent in our layer node map
    auto it = layerNodeMap_.find(parentId);
    return (it != layerNodeMap_.end()) ? it->second : nullptr;
  }

  void RenderListBuilder::updateLayerNodeProperties(std::shared_ptr<LayerNode> node, ecs::SystemContext &context)
  {
    auto webContent = context.getComponent<WebContent>(node->entityId);
    if (!webContent)
      return;

    // Update render queue properties
    auto renderQueue = context.getComponent<RenderQueue>(node->entityId);
    if (renderQueue)
    {
      node->zIndex = renderQueue->zIndex;
      node->translateZ = renderQueue->translateZ;
    }

    // Update clipping information from layout fragment
    const auto &fragment = webContent->fragment();
    if (fragment.has_value())
    {
      // Set clip rect based on content area
      float left = 0.0f;
      float top = 0.0f;
      float width = fragment->contentWidth();
      float height = fragment->contentHeight();

      node->clipRect = SkRect::MakeXYWH(left, top, width, height);

      // Check if rounded clipping is needed
      const auto &roundedRect = webContent->roundedRect();
      if (!roundedRect.isEmpty() && !roundedRect.isRect())
      {
        node->roundedClipRect = roundedRect;
        node->needsClipping = true;
      }
      else if (width > 0 && height > 0)
      {
        node->needsClipping = true;
      }
    }

    // Check for scrollable area
    auto scrollableArea = context.getComponent<client_scroll::ScrollableArea>(node->entityId);
    if (scrollableArea)
    {
      node->isScrollable = true;
      node->hasOverflow = scrollableArea->needsScrolling();

      glm::vec3 scrollOffset = scrollableArea->getScrollOffset();
      node->scrollOffset = scrollOffset;
    }

    // Update transform from Transform component if present
    auto transform = context.getComponent<Transform>(node->entityId);
    if (transform)
    {
      node->transform = transform->matrix();
    }
  }

  void RenderListBuilder::sortLayerChildren(std::shared_ptr<LayerNode> node)
  {
    std::sort(node->children.begin(), node->children.end(), [](const std::shared_ptr<LayerNode> &a, const std::shared_ptr<LayerNode> &b)
              {
                if (a->layerIndex != b->layerIndex)
                  return a->layerIndex < b->layerIndex;
                if (a->zIndex != b->zIndex)
                  return a->zIndex < b->zIndex;
                return a->translateZ < b->translateZ; });
  }

  void RenderListBuilder::traverseLayer(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands)
  {
    // Push transform if needed
    if (node->scrollOffset != glm::vec3(0.0f) || node->transform != glm::mat4(1.0f))
    {
      createTransformCommands(node, commands);
    }

    // Push clip if needed
    bool pushedClip = false;
    if (node->needsClipping)
    {
      createClipCommands(node, commands);
      pushedClip = true;
    }

    // Render this layer's WebContent
    RenderCommand renderCmd(RenderCommand::kRenderWebContent, node->entityId);
    renderCmd.layerNode = node;
    renderCmd.transform = node->getAbsoluteTransform();
    renderCmd.scrollOffset = node->getAccumulatedScrollOffset();
    commands.push_back(renderCmd);

    // Traverse children in order
    for (const auto &child : node->children)
    {
      traverseLayer(child, commands);
    }

    // Pop clip if we pushed one
    if (pushedClip)
    {
      RenderCommand popClipCmd(RenderCommand::kPopClip, node->entityId);
      commands.push_back(popClipCmd);
    }

    // Pop transform if we pushed one
    if (node->scrollOffset != glm::vec3(0.0f) || node->transform != glm::mat4(1.0f))
    {
      RenderCommand popTransformCmd(RenderCommand::kPopTransform, node->entityId);
      commands.push_back(popTransformCmd);
    }
  }

  void RenderListBuilder::createClipCommands(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands)
  {
    RenderCommand clipCmd(RenderCommand::kPushClip, node->entityId);
    clipCmd.layerNode = node;

    if (node->roundedClipRect.has_value())
    {
      clipCmd.roundedClipRect = node->roundedClipRect;
      clipCmd.useStencil = !shouldUseScissorClipping(node);
      clipCmd.useScissor = shouldUseScissorClipping(node);
    }
    else if (node->clipRect.has_value())
    {
      clipCmd.clipRect = node->clipRect;
      clipCmd.useScissor = true;
      clipCmd.useStencil = false;
    }

    commands.push_back(clipCmd);
  }

  void RenderListBuilder::createTransformCommands(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands)
  {
    RenderCommand transformCmd(RenderCommand::kPushTransform, node->entityId);
    transformCmd.layerNode = node;
    transformCmd.transform = node->transform;
    transformCmd.scrollOffset = node->scrollOffset;
    commands.push_back(transformCmd);
  }

  bool RenderListBuilder::shouldUseScissorClipping(std::shared_ptr<LayerNode> node) const
  {
    if (!config_.preferScissorClipping)
      return false;

    // Use scissor for simple rectangular clips
    if (!node->roundedClipRect.has_value())
      return true;

    // Check if rounded rect has negligible corner radii
    const auto &rrect = node->roundedClipRect.value();
    const float maxRadiusForScissor = 2.0f * config_.devicePixelRatio;

    for (int i = 0; i < 4; i++)
    {
      SkVector radius = rrect.radii(static_cast<SkRRect::Corner>(i));
      if (radius.x() > maxRadiusForScissor || radius.y() > maxRadiusForScissor)
        return false;
    }

    return true;
  }
}