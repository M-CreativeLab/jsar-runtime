#pragma once

#include <memory>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/scroll/layer_manager.hpp>

namespace builtin_scene
{
  namespace web_renderer
  {
    /**
     * Multi-layer rendering context that manages scrollable layers.
     * Extends the existing WebContentContext to support multiple layers.
     */
    class MultiLayerContext : public ecs::Resource
    {
    public:
      MultiLayerContext();
      ~MultiLayerContext() = default;

      // Layer management
      client_scroll::LayerManager& getLayerManager() { return layer_manager_; }
      const client_scroll::LayerManager& getLayerManager() const { return layer_manager_; }
      
      // Check if multi-layer rendering is enabled
      bool isMultiLayerEnabled() const { return multi_layer_enabled_; }
      void setMultiLayerEnabled(bool enabled) { multi_layer_enabled_ = enabled; }

    private:
      client_scroll::LayerManager layer_manager_;
      bool multi_layer_enabled_;
    };

    /**
     * System for managing multi-layer initialization and setup.
     */
    class MultiLayerInitSystem final : public ecs::System
    {
    public:
      using ecs::System::System;

    public:
      const std::string name() const override
      {
        return "web_render.MultiLayerInitSystem";
      }
      void onExecute() override;
    };

    /**
     * System for rendering content across multiple scrollable layers.
     * This system identifies scrollable containers and renders them to separate layers.
     */
    class MultiLayerRenderSystem final : public RenderBaseSystem
    {
    public:
      using RenderBaseSystem::RenderBaseSystem;

    public:
      const std::string name() const override
      {
        return "web_render.MultiLayerRenderSystem";
      }

    protected:
      void onExecute() override;
      void render(ecs::EntityId entity, WebContent &content) override;

    private:
      void processLayoutObjectForLayers(std::shared_ptr<client_layout::LayoutObject> layoutObject);
      void renderLayersInOrder();
      std::shared_ptr<MultiLayerContext> getMultiLayerContext();
      std::shared_ptr<client_layout::LayoutObject> getLayoutObjectFromEntity(ecs::EntityId entity);
      int calculateLayerDepth(std::shared_ptr<client_layout::LayoutObject> layoutObject);
    };
  }
}