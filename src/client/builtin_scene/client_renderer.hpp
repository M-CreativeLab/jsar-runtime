#pragma once

#include <iostream>
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
    void initializeMesh3d(std::shared_ptr<Mesh3d> mesh3d)
    {
      // client_graphics::WebGLState clientState = glContext_->clientState();
      auto vao = glContext_->createVertexArray();
      auto vbo = glContext_->createBuffer();
      auto ebo = glContext_->createBuffer();

      {
        // Bind the vertex array object, vertex buffer object, and element buffer object.
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, vao);
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, vbo);
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
      }
      mesh3d->initialize(glContext_, vao);
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
    void initializeMeshMaterial3d(std::shared_ptr<Mesh3d> mesh3d, std::shared_ptr<MeshMaterial3d> meshMaterial3d)
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

      // Configure the vertex data: vertex array object, vertex buffer object, and element buffer object.
      // Configure the vertex attributes and the vertex buffer data.
      {
        auto vao = mesh3d->vertexArrayObject();
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, vao);

        // Configure the vertex attributes
        auto configureAttribute = [this](const IVertexAttribute &attrib, int index, size_t stride, size_t offset)
        {
          glContext_->vertexAttribPointer(index,
                                          attrib.size(),
                                          attrib.type(),
                                          attrib.normalized(),
                                          stride,
                                          offset);
          glContext_->enableVertexAttribArray(index);
        };
        mesh3d->iterateEnabledAttributes(program, configureAttribute);

        // Configure the vertex buffer data
        auto &vertexBufferData = mesh3d->vertexBuffer().data();
        glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                               vertexBufferData.size(),
                               const_cast<uint8_t *>(vertexBufferData.data()),
                               client_graphics::WebGLBufferUsage::kStaticDraw);

        // Configure the element buffer object
        auto indices = mesh3d->indices();
        glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer,
                               indices.dataSize(),
                               indices.dataBuffer(),
                               client_graphics::WebGLBufferUsage::kStaticDraw);
      }

      // Configure the initial uniform values
      {
        client_graphics::WebGLProgramScope programScope(glContext_, program);
        updateTransformationMatrix(program, nullptr, true); // forcily update the transformation matrix.

        meshMaterial3d->initialize(glContext_, program, mesh3d);
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
      assert(mesh->initialized());
      assert(material->initialized());
      client_graphics::WebGLProgramScope programScope(glContext_, material->program());

      // Call lifecycle methods
      material->onBeforeDrawMesh(mesh);

      // Update matrices
      updateViewProjectionMatrix(programScope.program(), xrView);
      updateTransformationMatrix(programScope.program(), transform);

      // Draw the mesh
      {
        client_graphics::WebGLVertexArrayScope vaoScope(glContext_, mesh->vertexArrayObject());
        // TODO: support other primitive topologies?
        glContext_->drawElements(client_graphics::WebGLDrawMode::kTriangles,
                                 mesh->indices().size(),
                                 WEBGL_UNSIGNED_INT,
                                 0);
      }

      // Call lifecycle methods
      material->onAfterDrawMesh(mesh);
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
    const std::string name() const override { return "RenderSystem"; }
    void onExecute() override
    {
      auto renderer = getResource<Renderer>();
      assert(renderer != nullptr); // The renderer must be valid.

      auto xrExperience = getResource<WebXRExperience>();
      if (xrExperience != nullptr) // XR rendering
      {
        auto xrViewerPose = xrExperience->viewerPose();
        if (xrViewerPose != nullptr)
        {
          auto &views = xrViewerPose->views();
          for (auto view : views)
            render(*renderer, view);
          return;
        }
      }

      // Fallback to the default rendering
      render(*renderer);
    }

  private:
    /**
     * Render the scene with the given renderer.
     *
     * @param renderer The renderer to use.
     * @param view The XR view to render the scene with.
     */
    void render(Renderer &renderer, std::shared_ptr<client_xr::XRView> view = nullptr)
    {
      auto roots = queryEntities<hierarchy::Root>();
      if (roots.size() <= 0) // No root entities to render
        return;

      if (view != nullptr)
        renderer.setViewport(view->viewport());
      for (auto root : roots)
        traverseAndRender(root, renderer, view);
    }
    /**
     * Traverse the entity hierarchy and render the mesh with the given renderer in post-order.
     *
     * This method ensures that the child entities are rendered before the parent entity.
     *
     * @param entity The entity to traverse and render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the entity with.
     * @param isPostOrder Whether to render the entity in post-order, namely, render the child entities first.
     */
    void traverseAndRender(ecs::EntityId entity, Renderer &renderer,
                           std::shared_ptr<client_xr::XRView> view = nullptr,
                           bool isPostOrder = false)
    {
      auto renderEntity = [this, &renderer, view](ecs::EntityId entity)
      {
        // Render the mesh if it exists
        auto mesh = getComponent<Mesh3d>(entity);
        if (mesh != nullptr && !mesh->isRenderingDisabled())
          renderMesh(entity, mesh, renderer, view);

        // TODO: support other renderable components (e.g., particles, etc.)
      };

      if (!isPostOrder) // Pre-order rendering
        renderEntity(entity);

      auto children = getComponent<hierarchy::Children>(entity);
      if (children != nullptr)
      {
        for (auto child : children->children())
          traverseAndRender(child, renderer, view, isPostOrder);
      }

      if (isPostOrder) // Post-order rendering
        renderEntity(entity);
    }
    /**
     * Render the mesh with the given renderer.
     *
     * @param mesh The mesh entity to render.
     * @param renderer The renderer to use.
     * @param view The XR view to render the mesh with.
     */
    void renderMesh(ecs::EntityId &entity, std::shared_ptr<Mesh3d> meshComponent, Renderer &renderer,
                    std::shared_ptr<client_xr::XRView> view)
    {
      auto materialComponent = getComponent<MeshMaterial3d>(entity);
      if (materialComponent == nullptr)
        return;

      if (!meshComponent->initialized())
        renderer.initializeMesh3d(meshComponent);
      if (!materialComponent->initialized())
        renderer.initializeMeshMaterial3d(meshComponent, materialComponent);
      renderer.drawMesh3d(meshComponent,
                          materialComponent,
                          getComponent<Transform>(entity),
                          view);
    }
  };
}
