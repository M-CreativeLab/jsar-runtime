#pragma once

#include <memory>
#include <client/graphics/webgl_context.hpp>
#include "./ecs-inl.hpp"
#include "./mesh_material.hpp"

namespace builtin_scene
{
  class Renderer : public ecs::Resource
  {
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
    void initializeMeshMaterial(std::shared_ptr<MeshMaterial3d> material)
    {
      auto program = glContext_->createProgram();
      auto vertexShader = glContext_->createShader(client_graphics::WebGLShaderType::kVertex);
      auto fragmentShader = glContext_->createShader(client_graphics::WebGLShaderType::kFragment);
      glContext_->shaderSource(vertexShader, material->vertexShader().shader().source);
      glContext_->shaderSource(fragmentShader, material->fragmentShader().shader().source);
      glContext_->compileShader(vertexShader);
      glContext_->compileShader(fragmentShader);
      glContext_->attachShader(program, vertexShader);
      glContext_->attachShader(program, fragmentShader);
      glContext_->linkProgram(program);
      // TODO: update the program to the mesh material.
      material->setInitialized(true);
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
        if (materialComponent != nullptr)
        {
          if (!materialComponent->isInitialized())
            renderer->initializeMeshMaterial(materialComponent);
        }
      }
    }
  };
}
