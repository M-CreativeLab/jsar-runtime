#pragma once

#include <memory>
#include <span>
#include <client/graphics/webgl_context.hpp>
#include "./ecs-inl.hpp"
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
    void setViewport(client_xr::XRViewport &viewport)
    {
      glContext_->viewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }
    /**
     * Initialize the mesh with the given WebGL context, it will create the vertex array object,
     * vertex buffer object, and element buffer object, and upload the data to the GPU.
     *
     * @param mesh3d The mesh to initialize.
     */
    void initializeMesh3d(std::shared_ptr<Mesh3d> mesh3d)
    {
      // client_graphics::WebGLState clientState = glContext_->clientState();
      auto vao = glContext_->createVertexArray();
      auto vbo = glContext_->createBuffer();
      auto ebo = glContext_->createBuffer();

      {
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, vao);

        // Configure the vertex buffer object
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, vbo);
        for (auto &[attributeId, attributeData] : mesh3d->attributes())
        {
          glContext_->vertexAttribPointer(attributeId,
                                          attributeData->formatSize(),
                                          attributeData->formatType(),
                                          attributeData->normalized(),
                                          attributeData->stride(),
                                          attributeData->offset());
          glContext_->enableVertexAttribArray(attributeId);
        }

        auto &vertexBuffer = mesh3d->vertexBuffer();
        glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                               vertexBuffer.dataSize(),
                               vertexBuffer.dataBuffer(),
                               client_graphics::WebGLBufferUsage::kStaticDraw);

        // Configure the element buffer object
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
        auto indices = mesh3d->indices();
        glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer,
                               indices.dataSize(),
                               indices.dataBuffer(),
                               client_graphics::WebGLBufferUsage::kStaticDraw);
      }
      mesh3d->initialize(vao);
    }
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
    void initializeMeshMaterial3d(std::shared_ptr<MeshMaterial3d> meshMaterial3d)
    {
      auto program = glContext_->createProgram();
      auto vertexShader = glContext_->createShader(client_graphics::WebGLShaderType::kVertex);
      auto fragmentShader = glContext_->createShader(client_graphics::WebGLShaderType::kFragment);
      glContext_->shaderSource(vertexShader,
                               meshMaterial3d->getShaderSource(client_graphics::WebGLShaderType::kVertex));
      glContext_->shaderSource(fragmentShader,
                               meshMaterial3d->getShaderSource(client_graphics::WebGLShaderType::kFragment));
      glContext_->compileShader(vertexShader);
      glContext_->compileShader(fragmentShader);
      glContext_->attachShader(program, vertexShader);
      glContext_->attachShader(program, fragmentShader);
      glContext_->linkProgram(program);

      {
        client_graphics::WebGLProgramScope programScope(glContext_, program);
        updateTransformationMatrix(program, nullptr, true); // forcily update the transformation matrix.
        meshMaterial3d->initialize(glContext_, program);
      }
    }
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
                    std::shared_ptr<client_xr::XRView> xrView)
    {
      glContext_->enable(WEBGL_DEPTH_TEST);
      glContext_->depthMask(true);

      assert(mesh != nullptr && material != nullptr);
      client_graphics::WebGLProgramScope programScope(glContext_, material->program());

      // Update matrices
      updateViewProjectionMatrix(programScope.program(), xrView);
      updateTransformationMatrix(programScope.program(), transform);

      // Draw the mesh
      {
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, mesh->vertexArrayObject());
        glContext_->drawElements(client_graphics::WebGLDrawMode::kTriangles,
                                 mesh->indices().size(),
                                 WEBGL_UNSIGNED_INT,
                                 0);
      }
    }
    /**
     * Update the view projection matrix.
     *
     * @param program The WebGL program to update the view projection matrix with.
     * @param xrView The XR view to update the view projection matrix with.
     */
    void updateViewProjectionMatrix(std::shared_ptr<client_graphics::WebGLProgram> program,
                                    std::shared_ptr<client_xr::XRView> xrView)
    {
      assert(program != nullptr);

      auto viewProjectionLoc = glContext_->getUniformLocation(program, "viewProjection");
      if (!viewProjectionLoc.has_value())
        throw std::runtime_error("The viewProjection uniform location is not found.");

      auto handedness = MatrixHandedness::MATRIX_RIGHT_HANDED;
      if (xrView != nullptr && xrView->eye() == client_xr::XREye::kRight)
      {
        MatrixComputationGraph viewProjectionCG(WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye, handedness);
        glContext_->uniformMatrix4fv(viewProjectionLoc.value(), false, viewProjectionCG);
      }
      else
      {
        MatrixComputationGraph viewProjectionCG(WebGLMatrixPlaceholderId::ViewProjectionMatrix, handedness);
        glContext_->uniformMatrix4fv(viewProjectionLoc.value(), false, viewProjectionCG);
      }
    }
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
                                    bool forceUpdate = false)
    {
      assert(program != nullptr);

      glm::mat4 matToUpdate;
      if (transform == nullptr || !transform->isDirty())
      {
        if (!forceUpdate)
          return;

        if (transform != nullptr)
          matToUpdate = transform->matrix();
        else
          matToUpdate = Transform::Identity().matrix();
      }
      else
        matToUpdate = transform->matrix();

      auto loc = glContext_->getUniformLocation(program, "modelMatrix");
      if (!loc.has_value())
        throw std::runtime_error("The modelMatrix uniform location is not found.");
      glContext_->uniformMatrix4fv(loc.value(), false, matToUpdate);
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
  };

  class RenderSystem final : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute() override
    {
      auto renderer = getResource<Renderer>();
      assert(renderer != nullptr);

      auto meshes = queryEntities<Mesh3d>();
      auto xrExperience = getResource<WebXRExperience>();
      if (xrExperience != nullptr)
      {
        auto xrViewerPose = xrExperience->viewerPose();
        if (xrViewerPose != nullptr)
        {
          auto &views = xrViewerPose->views();
          for (auto view : views)
            render(meshes, *renderer, view);
          return;
        }
      }
      render(meshes, *renderer);
    }

  private:
    /**
     * Render the meshes with the given renderer.
     *
     * @param meshes The list of mesh entities to render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the meshes with.
     */
    void render(std::vector<builtin_scene::ecs::EntityId> &meshes,
                builtin_scene::Renderer &renderer,
                std::shared_ptr<client_xr::XRView> view = nullptr)
    {
      if (view != nullptr)
        renderer.setViewport(view->viewport());
      renderMeshes(meshes, renderer, view);
    }
    /**
     * Render the meshes with the given renderer.
     *
     * @param meshes The list of mesh entities to render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the meshes with.
     */
    void renderMeshes(std::vector<builtin_scene::ecs::EntityId> &meshes,
                      builtin_scene::Renderer &renderer,
                      std::shared_ptr<client_xr::XRView> view = nullptr)
    {
      for (auto &mesh : meshes)
        renderMesh(mesh, renderer, view);
    }
    /**
     * Render the mesh with the given renderer.
     *
     * @param mesh The mesh entity to render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the mesh with.
     */
    void renderMesh(builtin_scene::ecs::EntityId &mesh,
                    builtin_scene::Renderer &renderer,
                    std::shared_ptr<client_xr::XRView> view)
    {
      auto meshComponent = getComponent<Mesh3d>(mesh);
      auto materialComponent = getComponent<MeshMaterial3d>(mesh);
      if (meshComponent == nullptr || materialComponent == nullptr)
        return;

      if (!meshComponent->initialized())
        renderer.initializeMesh3d(meshComponent);
      if (!materialComponent->initialized())
        renderer.initializeMeshMaterial3d(materialComponent);
      renderer.drawMesh3d(meshComponent,
                          materialComponent,
                          getComponent<Transform>(mesh),
                          view);
    }
  };
}
