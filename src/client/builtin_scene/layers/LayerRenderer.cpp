#include "./LayerRenderer.hpp"
#include "../hierarchy.hpp"
#include "../instanced_mesh.hpp"
#include "../mesh_material.hpp"
#include "../materials.hpp"
#include <client/cssom/computed_style.hpp>

namespace builtin_scene::layers
{
  LayerRenderer::LayerRenderer(ecs::SystemContext &context)
      : ecs::System(context)
      , renderCommandsDirty_(true)
  {
    renderListBuilder_ = std::make_unique<RenderListBuilder>(config_);
    clipStack_ = std::make_unique<ClipStack>(config_);
    debugOverlay_ = std::make_unique<DebugOverlay>(config_);
  }

  void LayerRenderer::onExecute()
  {
    // Early exit if no WebContent entities need rendering
    if (!needsRendering())
      return;

    // Rebuild layer tree and render commands if needed
    if (renderCommandsDirty_ || renderListBuilder_->needsRebuild(getContext()))
    {
      renderListBuilder_->buildLayerTree(getContext());
      cachedRenderCommands_ = renderListBuilder_->generateRenderCommands();
      renderCommandsDirty_ = false;

      // Update debug overlay if enabled
      if (debugOverlay_->isEnabled())
      {
        debugOverlay_->collectDebugInfo(renderListBuilder_->getRootLayers());
      }
    }

    // Get canvas for rendering
    SkCanvas *canvas = getCanvas();
    if (!canvas)
      return;

    // Clear clip stack from previous frame
    clipStack_->clearAll(canvas);

    // Execute render commands in order
    for (const auto &command : cachedRenderCommands_)
    {
      executeRenderCommand(command, canvas);
    }

    // Render debug overlay if enabled
    if (debugOverlay_->isEnabled())
    {
      debugOverlay_->render(canvas, config_.devicePixelRatio);
    }

    // Clean up any remaining clip state
    clipStack_->clearAll(canvas);
  }

  void LayerRenderer::setConfig(const LayerRenderConfig &config)
  {
    config_ = config;
    renderCommandsDirty_ = true;

    // Update subsystems with new config
    renderListBuilder_ = std::make_unique<RenderListBuilder>(config_);
    clipStack_ = std::make_unique<ClipStack>(config_);
    debugOverlay_ = std::make_unique<DebugOverlay>(config_);
  }

  void LayerRenderer::setDebugOverlayEnabled(bool enabled)
  {
    config_.enableDebugOverlay = enabled;
    debugOverlay_->setEnabled(enabled);
  }

  bool LayerRenderer::isDebugOverlayEnabled() const
  {
    return debugOverlay_->isEnabled();
  }

  void LayerRenderer::executeRenderCommand(const RenderCommand &command, SkCanvas *canvas)
  {
    switch (command.type)
    {
    case RenderCommand::kPushClip:
      if (command.roundedClipRect.has_value())
      {
        clipStack_->pushClip(canvas, command.roundedClipRect.value());
      }
      else if (command.clipRect.has_value())
      {
        clipStack_->pushClip(canvas, command.clipRect.value());
      }
      break;

    case RenderCommand::kPopClip:
      clipStack_->popClip(canvas);
      break;

    case RenderCommand::kPushTransform:
      canvas->save();
      // Apply scroll offset as translation
      canvas->translate(-command.scrollOffset.x, -command.scrollOffset.y);
      // Apply additional transform if not identity
      if (command.transform != glm::mat4(1.0f))
      {
        // Convert glm::mat4 to SkMatrix (simplified for 2D case)
        SkMatrix transform;
        transform.setAll(
          command.transform[0][0], command.transform[0][1], command.transform[0][3], command.transform[1][0], command.transform[1][1], command.transform[1][3], command.transform[3][0], command.transform[3][1], command.transform[3][3]);
        canvas->concat(transform);
      }
      break;

    case RenderCommand::kPopTransform:
      canvas->restore();
      break;

    case RenderCommand::kRenderWebContent:
      renderWebContent(command.entityId, canvas);
      break;
    }
  }

  void LayerRenderer::renderWebContent(ecs::EntityId entityId, SkCanvas *canvas)
  {
    auto webContent = getComponent<WebContent>(entityId);
    if (!webContent || !webContent->isEnabled() || !webContent->isDirty())
      return;

    // Get the WebContent's own canvas and render to it
    SkCanvas *webContentCanvas = webContent->canvas();
    if (!webContentCanvas)
      return;

    // Clear the WebContent surface
    webContentCanvas->clear(SK_ColorTRANSPARENT);

    // The individual rendering systems (background, image, text) will
    // render to the WebContent's canvas. We just need to ensure they
    // are called in the correct order for this layer.

    // Note: The actual rendering is delegated to the existing systems
    // (RenderBackgroundSystem, RenderImageSystem, RenderTextSystem)
    // which will be called after this system in the ECS schedule.
    // This system primarily manages the layering order and clipping.
  }

  bool LayerRenderer::needsRendering() const
  {
    auto webContentEntities = getContext().queryEntitiesWithComponent<WebContent>();

    for (const auto &[entityId, webContent] : webContentEntities)
    {
      if (webContent->isEnabled() && webContent->isDirty())
        return true;
    }

    return false;
  }

  SkCanvas *LayerRenderer::getCanvas()
  {
    // For now, we'll work with the individual WebContent canvases
    // In a future iteration, this could be enhanced to support
    // a unified canvas for all layers with proper compositing
    return nullptr;
  }

  // LayeredWebContentRenderSystem implementation

  LayeredWebContentRenderSystem::LayeredWebContentRenderSystem(ecs::SystemContext &context)
      : ecs::System(context)
  {
    layerRenderer_ = std::make_unique<LayerRenderer>(context);
  }

  void LayeredWebContentRenderSystem::onExecute()
  {
    // Execute the layer-aware rendering
    layerRenderer_->onExecute();

    // The layer renderer manages the order and clipping,
    // but the actual rendering to individual WebContent surfaces
    // is still handled by the existing rendering systems
    // (RenderBackgroundSystem, RenderImageSystem, RenderTextSystem)
    // which should run after this system.
  }
}