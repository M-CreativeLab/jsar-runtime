#pragma once

#include <memory>
#include <span>
#include <client/graphics/webgl_context.hpp>
#include "./ecs-inl.hpp"
#include "./mesh_material.hpp"
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
    std::shared_ptr<client_graphics::WebGL2Context> glContext() const
    {
      return glContext_;
    }
    void setViewport(client_xr::XRViewport &viewport)
    {
      glContext_->viewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }
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
    void initializeMeshMaterial3d(std::shared_ptr<MeshMaterial3d> meshMaterial3d)
    {
      auto program = glContext_->createProgram();
      auto vertexShader = glContext_->createShader(client_graphics::WebGLShaderType::kVertex);
      auto fragmentShader = glContext_->createShader(client_graphics::WebGLShaderType::kFragment);
      glContext_->shaderSource(vertexShader, meshMaterial3d->vertexShader().shader().source);
      glContext_->shaderSource(fragmentShader, meshMaterial3d->fragmentShader().shader().source);
      glContext_->compileShader(vertexShader);
      glContext_->compileShader(fragmentShader);
      glContext_->attachShader(program, vertexShader);
      glContext_->attachShader(program, fragmentShader);
      glContext_->linkProgram(program);

      {
        client_graphics::WebGLProgramScope programScope(glContext_, program);
        auto viewProjectionLoc = glContext_->getUniformLocation(program, "viewProjection");
        auto modelMatrixLoc = glContext_->getUniformLocation(program, "modelMatrix");
        {
          glm::mat4 identity(1.0f);
          std::vector<float> modelMatrix;
          for (int i = 0; i < 16; i++)
            modelMatrix.push_back(identity[i / 4][i % 4]);

          MatrixComputationGraph viewProjectionCG(WebGLMatrixPlaceholderId::ViewProjectionMatrix, MatrixHandedness::MATRIX_RIGHT_HANDED);
          glContext_->uniformMatrix4fv(viewProjectionLoc.value(), false, viewProjectionCG);
          glContext_->uniformMatrix4fv(modelMatrixLoc.value(), false, modelMatrix);

          auto surfaceColorLoc = glContext_->getUniformLocation(program, "surfaceColor");
          glm::vec4 surfaceColor(1.0f, 1.0f, 1.0f, 1.0f);
          glContext_->uniform4f(surfaceColorLoc.value(), surfaceColor.r, surfaceColor.g, surfaceColor.b, surfaceColor.a);
        }
      }
      meshMaterial3d->initialize(program);
    }
    void renderMesh3d(std::shared_ptr<Mesh3d> mesh, std::shared_ptr<MeshMaterial3d> material, std::shared_ptr<client_xr::XRView> xrView)
    {
      glContext_->enable(WEBGL_DEPTH_TEST);
      glContext_->depthMask(true);

      assert(mesh != nullptr && material != nullptr);
      client_graphics::WebGLProgramScope programScope(glContext_, material->program());

      auto viewProjectionLoc = glContext_->getUniformLocation(programScope.program(), "viewProjection");
      if (xrView != nullptr && xrView->eye() == client_xr::XREye::kRight)
      {
        MatrixComputationGraph viewProjectionCG(WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye, MatrixHandedness::MATRIX_RIGHT_HANDED);
        glContext_->uniformMatrix4fv(viewProjectionLoc.value(), false, viewProjectionCG);
      }
      else
      {
        MatrixComputationGraph viewProjectionCG(WebGLMatrixPlaceholderId::ViewProjectionMatrix, MatrixHandedness::MATRIX_RIGHT_HANDED);
        glContext_->uniformMatrix4fv(viewProjectionLoc.value(), false, viewProjectionCG);
      }

      // Draw the mesh
      {
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, mesh->vertexArrayObject());
        glContext_->drawElements(client_graphics::WebGLDrawMode::kTriangles,
                                 mesh->indices().size(),
                                 WEBGL_UNSIGNED_INT,
                                 0);
      }
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
  };

  class RenderSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute()
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
            renderMeshes(meshes, *renderer, view);
          return;
        }
      }
      renderMeshes(meshes, *renderer);
    }

  private:
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
      if (view != nullptr)
        renderer.setViewport(view->viewport());
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
      renderer.renderMesh3d(meshComponent, materialComponent, view);
    }
  };
}
