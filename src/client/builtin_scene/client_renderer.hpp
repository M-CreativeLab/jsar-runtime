#pragma once

#include <iostream>
#include <memory>
#include <span>
#include <client/graphics/webgl_context.hpp>
#include "./ecs-inl.hpp"
#include "./meshes.hpp"
#include "./mesh_material.hpp"
#include "./transform.hpp"
#include "./xr.hpp"

namespace builtin_scene
{
  class Renderer : public ecs::Resource
  {
    friend class RendererScope;
    friend class RenderSystem;

  public:
    Renderer(std::shared_ptr<client_graphics::WebGL2Context> glContext)
        : glContext_(glContext)
    {
      glContext_->enable(WEBGL_CULL_FACE);
    }

  public:
    /**
     * @returns The WebGL context of the renderer.
     */
    std::shared_ptr<client_graphics::WebGL2Context> glContext() const
    {
      return glContext_;
    }
    /**
     * Set the viewport of the renderer.
     *
     * @param viewport The viewport to set.
     */
    inline void setViewport(client_xr::XRViewport &viewport)
    {
      glContext_->viewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }
    /**
     * Set polygon offset enabled or disabled.
     *
     * @param enabled Whether to enable the stencil write mask.
     */
    void setPolygonOffset(bool enabled = true)
    {
      if (enabled)
      {
        glContext_->enable(WEBGL_POLYGON_OFFSET_FILL);
        glContext_->polygonOffset(-1.0f, -1.0f);
      }
      else
      {
        glContext_->disable(WEBGL_POLYGON_OFFSET_FILL);
      }
    }
    /**
     * Initialize the mesh with the given WebGL context, it will create the vertex array object,
     * vertex buffer object, and element buffer object, and upload the data to the GPU.
     *
     * @param mesh3d The mesh to initialize.
     */
    void initializeMesh3d(std::shared_ptr<Mesh3d> mesh3d);
    /**
     * Initialize the mesh material with the given WebGL context, it will create the program, shaders
     * and link the program.
     *
     * It will also initialize the `MeshMaterial3d` instance after the program is created, it will
     * set the initial transformation matrix to the program, set the default uniform values of the
     * related fragment shader.
     *
     * @param meshMaterial3d The mesh material to initialize.
     */
    void initializeMeshMaterial3d(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<MeshMaterial3d> meshMaterial3d);
    /**
     * Draw the `Mesh3d` with the given `MeshMaterial3d` and `XRView`.
     *
     * @param mesh The mesh to draw.
     * @param material The material to draw the mesh with.
     * @param transform The transform of the mesh.
     * @param xrView The XR view to draw the mesh with.
     */
    void drawMesh3d(std::shared_ptr<Mesh3d> mesh, std::shared_ptr<MeshMaterial3d> material,
                    std::shared_ptr<Transform> transform,
                    std::shared_ptr<client_xr::XRView> xrView);
    /**
     * Update the view projection matrix.
     *
     * @param program The WebGL program to update the view projection matrix with.
     * @param xrView The XR view to update the view projection matrix with.
     */
    void updateViewProjectionMatrix(std::shared_ptr<client_graphics::WebGLProgram> program,
                                    std::shared_ptr<client_xr::XRView> xrView);
    /**
     * Update the transformation matrix for the given program and mesh.
     *
     * If the `forceUpdate` is `false`, this method will only update the transformation matrix if the
     * transform is dirty to avoid unnecessary updates. Otherwise, it will always update the transformation
     * matrix.
     *
     * @param program The WebGL program to update the transformation matrix with.
     * @param transform The transform to update the transformation matrix with, `nullptr` for the identity matrix.
     * @param forceUpdate Whether to force update the transformation matrix.
     */
    void updateTransformationMatrix(std::shared_ptr<client_graphics::WebGLProgram> program,
                                    std::shared_ptr<Transform> transform,
                                    bool forceUpdate = false);

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
  };

  class RenderSystem final : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    const std::string name() const override { return "RenderSystem"; }
    void onExecute() override;

  private:
    /**
     * Render the scene with the given renderer.
     *
     * @param renderer The renderer to use.
     * @param view The XR view to render the scene with.
     */
    void render(Renderer &renderer, std::shared_ptr<client_xr::XRView> view = nullptr);
    /**
     * Traverse the entity hierarchy and render the mesh with the given renderer in pre-order.
     *
     * @param entity The entity to traverse and render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the entity with.
     * @param isPostOrder Whether to render the entity in post-order, namely, render the child entities first.
     */
    void traverseAndRender(ecs::EntityId entity, Renderer &renderer,
                           std::shared_ptr<client_xr::XRView> view = nullptr);
    /**
     * Render the mesh with the given renderer.
     *
     * @param mesh The mesh entity to render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the mesh with.
     */
    void renderMesh(ecs::EntityId &entity, std::shared_ptr<Mesh3d> meshComponent, Renderer &renderer,
                    std::shared_ptr<client_xr::XRView> view);
  };
}
