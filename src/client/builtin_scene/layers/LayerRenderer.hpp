#pragma once

#include <memory>
#include <vector>
#include "./LayerTypes.hpp"
#include "./RenderListBuilder.hpp"
#include "./ClipStack.hpp"
#include "./DebugOverlay.hpp"
#include "../ecs.hpp"
#include "../web_content.hpp"

namespace builtin_scene::layers
{
  /**
   * Main layer rendering system that orchestrates layered WebContent rendering.
   * Integrates RenderListBuilder, ClipStack, and DebugOverlay to provide
   * correct painter's order rendering with nested scrollable containers.
   */
  class LayerRenderer : public ecs::System
  {
  public:
    LayerRenderer(ecs::SystemContext &context);
    ~LayerRenderer() = default;

    // ecs::System interface
    const std::string name() const override
    {
      return "layers.LayerRenderer";
    }

    void onExecute() override;

    /**
     * Configure layer rendering behavior.
     */
    void setConfig(const LayerRenderConfig &config);
    const LayerRenderConfig &getConfig() const
    {
      return config_;
    }

    /**
     * Enable or disable debug overlay.
     */
    void setDebugOverlayEnabled(bool enabled);
    bool isDebugOverlayEnabled() const;

  private:
    /**
     * Execute a single render command with proper state management.
     */
    void executeRenderCommand(const RenderCommand &command, SkCanvas *canvas);

    /**
     * Render a WebContent entity with current transform and clip state.
     */
    void renderWebContent(ecs::EntityId entityId, SkCanvas *canvas);

    /**
     * Check if any WebContent entities need re-rendering.
     */
    bool needsRendering() const;

    /**
     * Get the canvas for rendering. Handles both mono and stereo modes.
     */
    SkCanvas *getCanvas();

  private:
    LayerRenderConfig config_;
    std::unique_ptr<RenderListBuilder> renderListBuilder_;
    std::unique_ptr<ClipStack> clipStack_;
    std::unique_ptr<DebugOverlay> debugOverlay_;

    // Cached render commands (rebuilt when layer tree changes)
    std::vector<RenderCommand> cachedRenderCommands_;
    bool renderCommandsDirty_;
  };

  /**
   * System that replaces the existing WebContent rendering pipeline
   * with the new layered approach. Integrates with the existing ECS
   * rendering systems.
   */
  class LayeredWebContentRenderSystem : public ecs::System
  {
  public:
    LayeredWebContentRenderSystem(ecs::SystemContext &context);
    ~LayeredWebContentRenderSystem() = default;

    // ecs::System interface
    const std::string name() const override
    {
      return "web_renderer.LayeredRenderSystem";
    }

    void onExecute() override;

  private:
    std::unique_ptr<LayerRenderer> layerRenderer_;
  };
}