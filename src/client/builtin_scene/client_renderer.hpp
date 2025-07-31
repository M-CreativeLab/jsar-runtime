#pragma once

#include <iostream>
#include <memory>

#include "./ecs-inl.hpp"
#include "./meshes.hpp"
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
     * Get the transformation matrix of the entity.
     *
     * @returns The transformation matrix of the entity.
     */
    glm::mat4 getTransformationMatrix(ecs::EntityId id);
    /**
     * Update the instance data for the mesh if it's an instanced mesh.
     *
     * @param meshComponent The mesh component to update the instance data with.
     */
    void tryUpdateInstanceDataForInstancedMesh(const Mesh3d &meshComponent,
                                               std::optional<XRRenderTarget> renderTarget);
    /**
     * Render the scene with the given renderer.
     *
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void render(SceneRenderer &, std::optional<XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Render the volume mask with the given renderer.
     *
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void renderVolumeMask(SceneRenderer &, std::optional<XRRenderTarget> renderTarget);
    /**
     * Execute this before rendering the scene.
     *
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void onBeforeRender(SceneRenderer &, std::optional<XRRenderTarget> renderTarget);
    /**
     * Execute this after rendering the scene.
     *
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void onAfterRender(SceneRenderer &, std::optional<XRRenderTarget> renderTarget);
    /**
     * Traverse the entity hierarchy and render the mesh with the given renderer in pre-order.
     *
     * @param entity The entity to traverse and render.
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void traverseAndRender(ecs::EntityId, SceneRenderer &, std::optional<XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Render the mesh with the given renderer.
     *
     * @param entity The entity to render.
     * @param meshComponent The mesh component to render.
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void renderMesh(ecs::EntityId &entity,
                    std::shared_ptr<Mesh3d> meshComponent,
                    SceneRenderer &renderer,
                    std::optional<XRRenderTarget> renderTarget);
  };
}
