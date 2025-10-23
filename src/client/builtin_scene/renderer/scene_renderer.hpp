#pragma once

#include <iostream>
#include <memory>
#include <math/vectors.hpp>

#include <client/graphics/webgl_context.hpp>
#include <client/builtin_scene/ecs-inl.hpp>
#include <client/builtin_scene/meshes.hpp>
#include <client/builtin_scene/mesh_material.hpp>
#include <client/builtin_scene/gaussian_splats_mesh.hpp>
#include <client/builtin_scene/transform.hpp>
#include <client/builtin_scene/xr.hpp>

#include "./render_pass.hpp"
#include "./render_target.hpp"

namespace endor
{
  namespace builtin_scene
  {
    class SceneRenderer : public ecs::Resource
    {
      friend class RendererScope;
      friend class RenderSystem;

    public:
      SceneRenderer(std::shared_ptr<client_graphics::WebGL2Context> glContext, math::Size3 volumeSize);

    public:
      /**
     * @returns The WebGL context of the renderer.
     */
      inline std::shared_ptr<client_graphics::WebGL2Context> glContext() const
      {
        return glContext_;
      }
      /**
     * Set the viewport of the renderer.
     *
     * @param viewport The viewport to set.
     */
      void setViewport(client_xr::XRViewport &viewport);
      /**
     * Set polygon offset enabled or disabled.
     *
     * @param enabled Whether to enable the stencil write mask.
     */
      void setPolygonOffset(bool enabled = true);
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
     * @param renderPass The render pass to use, which can be used to filter the objects or instances to render.
     * @param renderTarget The XR render target to draw the mesh with.
     */
      void drawMesh3d(const ecs::EntityId &entity,
                      std::shared_ptr<Mesh3d> mesh,
                      std::shared_ptr<MeshMaterial3d> material,
                      std::shared_ptr<Transform> transform,
                      std::shared_ptr<Transform> parentTransform,
                      RenderPass renderPass,
                      std::optional<XRRenderTarget> renderTarget);
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

    public:
      void onBeforeRender(const RenderPass renderPass, std::optional<XRRenderTarget> renderTarget);
      void onAfterRender(const RenderPass renderPass, std::optional<XRRenderTarget> renderTarget);

    private:
      std::shared_ptr<client_graphics::WebGL2Context> glContext_;
      math::Size3 volumeSize_;
    };
  }
} // namespace endor
