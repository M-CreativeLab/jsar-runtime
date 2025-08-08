#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include "./LayerTypes.hpp"
#include "../ecs.hpp"

namespace builtin_scene::layers
{
  /**
   * Builds an ordered list of render commands from the layer tree.
   * Implements depth-first traversal to ensure correct painter's order
   * and proper clipping state management.
   */
  class RenderListBuilder
  {
  public:
    RenderListBuilder(const LayerRenderConfig &config);
    ~RenderListBuilder() = default;

    /**
     * Build the layer tree from the current ECS state.
     * Queries all WebContent entities and constructs the hierarchical layer tree.
     */
    void buildLayerTree(ecs::SystemContext &context);

    /**
     * Generate the ordered render command list from the layer tree.
     * Performs depth-first traversal starting from root layers.
     */
    std::vector<RenderCommand> generateRenderCommands();

    /**
     * Get the root layer nodes (layers with no parent).
     */
    const std::vector<std::shared_ptr<LayerNode>> &getRootLayers() const
    {
      return rootLayers_;
    }

    /**
     * Get a specific layer node by entity ID.
     */
    std::shared_ptr<LayerNode> getLayerNode(ecs::EntityId entityId) const;

    /**
     * Check if the layer tree needs to be rebuilt.
     * Returns true if any WebContent entities have been added, removed, or had their layer changed.
     */
    bool needsRebuild(ecs::SystemContext &context) const;

    /**
     * Clear the current layer tree and command list.
     */
    void clear();

  private:
    /**
     * Create a layer node from a WebContent entity.
     */
    std::shared_ptr<LayerNode> createLayerNode(ecs::EntityId entityId, ecs::SystemContext &context);

    /**
     * Determine the parent layer for a given entity based on hierarchy and layer indices.
     */
    std::shared_ptr<LayerNode> findParentLayer(ecs::EntityId entityId, ecs::SystemContext &context);

    /**
     * Update layer node properties from WebContent and related components.
     */
    void updateLayerNodeProperties(std::shared_ptr<LayerNode> node, ecs::SystemContext &context);

    /**
     * Sort children layers by their rendering order (layer index, z-index, translateZ).
     */
    void sortLayerChildren(std::shared_ptr<LayerNode> node);

    /**
     * Recursively traverse the layer tree and generate render commands.
     */
    void traverseLayer(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands);

    /**
     * Create clip commands for a layer if it needs clipping.
     */
    void createClipCommands(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands);

    /**
     * Create transform commands for a layer.
     */
    void createTransformCommands(std::shared_ptr<LayerNode> node, std::vector<RenderCommand> &commands);

    /**
     * Determine if a layer should use scissor or stencil clipping.
     */
    bool shouldUseScissorClipping(std::shared_ptr<LayerNode> node) const;

  private:
    const LayerRenderConfig &config_;
    std::vector<std::shared_ptr<LayerNode>> rootLayers_;
    std::unordered_map<ecs::EntityId, std::shared_ptr<LayerNode>> layerNodeMap_;
    std::unordered_map<ecs::EntityId, int> lastSeenLayerIndex_;
    bool isDirty_;
  };
}