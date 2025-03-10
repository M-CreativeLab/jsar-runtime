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

  private:
    /**
     * The WebXR render target for the renderer, which stores the view or views to render.
     */
    class XRRenderTarget
    {
    public:
      /**
       * Construct the render target for the single view.
       *
       * @param view The view to render.
       */
      XRRenderTarget(std::shared_ptr<client_xr::XRView> view)
          : multiview_(false), view_(view)
      {
      }
      /**
       * Construct the render target for the multiple views.
       *
       * @param views The views to render.
       */
      XRRenderTarget(const std::vector<std::shared_ptr<client_xr::XRView>> &views)
          : multiview_(true), views_(&views)
      {
      }

    public:
      /**
       * @returns Whether the render target is multiview.
       */
      inline bool isMultiview() const { return multiview_; }
      /**
       * @returns The view to render, it is `nullptr` if the render target is multiview.
       */
      inline std::shared_ptr<client_xr::XRView> view() const { return view_; }
      /**
       * @returns The views to render, and it will throw an exception if the render target is not multiview.
       */
      inline const std::vector<std::shared_ptr<client_xr::XRView>> &views() const
      {
        if (views_ == nullptr)
          throw std::runtime_error("The render target is not multiview.");
        return *views_;
      }

    private:
      bool multiview_ = false;
      std::shared_ptr<client_xr::XRView> view_ = nullptr;
      const std::vector<std::shared_ptr<client_xr::XRView>> *views_ = nullptr;
    };

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
     * Configure the mesh vertex data with the given WebGL context, it will upload the dirty vertex data
     * to the GPU.
     *
     * @param mesh3d The mesh to configure the vertex data.
     * @param program The WebGL program to configure the vertex data with.
     */
    void configureMeshVertexData(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<client_graphics::WebGLProgram> program);
    void updateMeshVertexData(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<client_graphics::WebGLProgram> program);
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
     * Try to update the mesh material:
     *
     * 1. update the mesh vertext data if it's dirty.
     * 2. update the instanced data like color, texture or other attributes.
     */
    void tryUpdateMeshMaterial3d(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<MeshMaterial3d> meshMaterial3d);
    /**
     * Draw the `Mesh3d` with the given `MeshMaterial3d` and `XRView`.
     *
     * @param mesh The mesh to draw.
     * @param material The material to draw the mesh with.
     * @param transform The transform of the mesh.
     * @param parentTransform The parent transform of the mesh, `nullptr` for the root transform.
     * @param renderTarget The XR render target to draw the mesh with.
     */
    void drawMesh3d(const ecs::EntityId &entity, std::shared_ptr<Mesh3d> mesh, std::shared_ptr<MeshMaterial3d> material,
                    std::shared_ptr<Transform> transform,
                    std::shared_ptr<Transform> parentTransform = nullptr,
                    std::optional<XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Update the view projection matrix.
     *
     * @param program The WebGL program to update the view projection matrix with.
     * @param renderTarget The XR render target to update the view projection matrix with.
     */
    void updateViewProjectionMatrix(std::shared_ptr<client_graphics::WebGLProgram> program,
                                    std::optional<XRRenderTarget> renderTarget);
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
    std::optional<glm::mat4> updateTransformationMatrix(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                        std::shared_ptr<Transform> transform,
                                                        std::shared_ptr<Transform> parentTransform = nullptr,
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
     * Get the transformation matrix of the entity.
     * 
     * @returns The transformation matrix of the entity.
     */
    glm::mat4 getTransformationMatrix(ecs::EntityId id);
    /**
     * Render the scene with the given renderer.
     *
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void render(Renderer &renderer, std::optional<Renderer::XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Traverse the entity hierarchy and render the mesh with the given renderer in pre-order.
     *
     * @param entity The entity to traverse and render.
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void traverseAndRender(ecs::EntityId entity, Renderer &renderer,
                           std::optional<Renderer::XRRenderTarget> renderTarget = std::nullopt);
    /**
     * Render the mesh with the given renderer.
     *
     * @param entity The entity to render.
     * @param meshComponent The mesh component to render.
     * @param renderer The renderer to use.
     * @param renderTarget The XR render target.
     */
    void renderMesh(ecs::EntityId &entity, std::shared_ptr<Mesh3d> meshComponent, Renderer &renderer,
                    std::optional<Renderer::XRRenderTarget> renderTarget);

  };
}
