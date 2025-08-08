#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "./ecs.hpp"
#include "./web_content.hpp"
#include "./model_3d.hpp"
#include "./client_renderer.hpp"
#include "../graphics/webgl_context.hpp"

namespace builtin_scene::model_renderer
{
  /**
   * System to initialize 3D model rendering infrastructure.
   * Similar to InitSystem in web_content_renderer.cpp
   */
  class InitSystem : public ecs::System
  {
    using ecs::System::System;

  public:
    const std::string name() const override
    {
      return "model_renderer.InitSystem";
    }
    void onExecute() override;
  };

  /**
   * Base system for 3D model rendering operations.
   * Follows the same pattern as RenderBaseSystem in web_content_renderer.cpp
   */
  class RenderBaseSystem : public ecs::System
  {
    using ecs::System::System;

  protected:
    virtual void render(ecs::EntityId entity);
  };

  /**
   * System to render 3D Gaussian Splatting models using WebGL.
   * This is the core client-side renderer for 3DGS content.
   */
  class RenderGaussianSplattingSystem : public RenderBaseSystem
  {
    using RenderBaseSystem::RenderBaseSystem;

  public:
    const std::string name() const override
    {
      return "model_renderer.RenderGaussianSplattingSystem";
    }
    void onExecute() override;

  private:
    void render(ecs::EntityId entity) override;
    void render(ecs::EntityId entity, std::shared_ptr<client_graphics::WebGL2Context> glContext);
  };

  /**
   * Plugin for 3D model rendering systems.
   * This plugin registers all the systems needed for 3D model rendering,
   * following the same pattern as WebContentPlugin.
   */
  class Model3dPlugin final : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &app) override
    {
      using namespace ecs;

      // Register the Model3d component
      app.registerComponent<Model3d>();

      // Register systems in rendering order
      auto initModel = System::Make<InitSystem>();
      app.addSystem(SchedulerLabel::kPostStartup, initModel);

      auto render3DGS = System::Make<RenderGaussianSplattingSystem>();
      app.addSystem(SchedulerLabel::kUpdate, render3DGS);
    }
  };
}