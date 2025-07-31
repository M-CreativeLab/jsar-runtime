#pragma once

#include <iostream>
#include <memory>

#include "./ecs-inl.hpp"
#include "./meshes.hpp"
#include "./xr.hpp"

#include "./renderer/render_pass.hpp"
#include "./renderer/scene_renderer.hpp"

namespace builtin_scene
{
  class RenderStartupSystem final : public ecs::System
  {
    using ecs::System::System;

  public:
    const std::string name() const override
    {
      return "RenderStartupSystem";
    }
    void onExecute() override;
  };

  class RenderSystem final : public ecs::System
  {
    using ecs::System::System;

  public:
    const std::string name() const override
    {
      return "RenderSystem";
    }
    void onExecute() override;

  private:
    /**
     * Render the scene with the given renderer.
     *
     * @param renderTarget The XR render target.
     */
    void render(std::optional<XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Render objects and instances in the given render pass.
     * 
     * @param roots The root entities to traverse and render.
     * @param renderPass The render pass to use, which can be used to filter the objects or instances to render.
     * @param renderTarget The XR render target to draw to.
     */
    void renderPass(vector<ecs::EntityId> &roots, RenderPass, optional<XRRenderTarget>);
    /**
     * Render the volume mask with the given renderer.
     *
     * @param renderTarget The XR render target.
     */
    void renderVolumeMask(RenderPass, std::optional<XRRenderTarget>);
    /**
     * Render the mesh with the given renderer.
     *
     * @param entity The entity to render.
     * @param meshComponent The mesh component to render.
     * @param materialComponent The mesh material component to use for rendering.
     * @param renderPass The render pass to use, which can be used to filter the objects or instances to render.
     * @param renderTarget The XR render target.
     */
    void renderMesh(const ecs::EntityId &,
                    std::shared_ptr<Mesh3d>,
                    std::shared_ptr<MeshMaterial3d>,
                    const RenderPass,
                    std::optional<XRRenderTarget>);

    void onBeforeRender(std::optional<XRRenderTarget>);
    void onAfterRender(std::optional<XRRenderTarget>);

    // Traverse the entity hierarchy in pre-order and execute the given function for each entity.
    void traverse(ecs::EntityId root, std::function<bool(ecs::EntityId)> &&);
    size_t traverseAndUpdate(ecs::EntityId root, std::optional<XRRenderTarget>);

    glm::mat4 getTransformationMatrix(ecs::EntityId);
    void updateInstancedMeshData(const Mesh3d &, std::optional<XRRenderTarget>);

  private:
    std::shared_ptr<SceneRenderer> renderer_ = nullptr;
    std::shared_ptr<WebXRExperience> xrExperience_ = nullptr;
  };
}
