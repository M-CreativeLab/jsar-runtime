#pragma once

#include <memory>
#include <client/graphics/webgl_context.hpp>
#include "./ecs-inl.hpp"
#include "./mesh_material.hpp"

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
    void initializeMesh3d(std::shared_ptr<Mesh3d> mesh3d)
    {
      client_graphics::WebGLState clientState = glContext_->clientState();
      auto vao = glContext_->createVertexArray();
      auto vbo = glContext_->createBuffer();
      auto ebo = glContext_->createBuffer();

      glContext_->bindVertexArray(vao);
      {
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, vbo);
        {
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

          auto indices = mesh3d->indices();
          glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, ebo);
          glContext_->bufferData(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer,
                                 indices.size() * sizeof(uint32_t),
                                 indices.data(),
                                 client_graphics::WebGLBufferUsage::kStaticDraw);
        }
        glContext_->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, nullptr);
      }
      glContext_->bindVertexArray(nullptr);
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
      meshMaterial3d->initialize(program);
    }
    void renderMesh3d(std::shared_ptr<Mesh3d> mesh, std::shared_ptr<MeshMaterial3d> material)
    {
      assert(mesh != nullptr && material != nullptr);
        // TODO: Call the draw function
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
      for (auto &mesh : meshes)
      {
        auto meshComponent = getComponent<Mesh3d>(mesh);
        auto materialComponent = getComponent<MeshMaterial3d>(mesh);
        if (meshComponent == nullptr || materialComponent == nullptr)
          continue;

        if (!meshComponent->initialized())
          renderer->initializeMesh3d(meshComponent);
        if (!materialComponent->initialized())
          renderer->initializeMeshMaterial3d(materialComponent);
        renderer->renderMesh3d(meshComponent, materialComponent);
      }
    }
  };
}
