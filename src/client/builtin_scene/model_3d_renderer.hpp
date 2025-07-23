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
  public:
    void onExecute() override;
  };

  /**
   * Base system for 3D model rendering operations.
   * Follows the same pattern as RenderBaseSystem in web_content_renderer.cpp
   */
  class RenderBaseSystem : public ecs::System
  {
  protected:
    void render(ecs::EntityId entity, WebContent &content);
  };

  /**
   * System to load 3D model files (GLTF, GLB, 3DGS formats).
   * Handles asynchronous loading and file format detection.
   */
  class LoadModelSystem : public RenderBaseSystem
  {
  public:
    void onExecute() override;

  private:
    void render(ecs::EntityId entity, WebContent &content) override;
    
    // File format detection
    Model3d::ModelType detectModelType(const std::string &src, const std::string &typeHint = "");
  };

  /**
   * System to render 3D Gaussian Splatting models using WebGL.
   * This is the core client-side renderer for 3DGS content.
   */
  class RenderGaussianSplattingSystem : public RenderBaseSystem
  {
  public:
    RenderGaussianSplattingSystem();
    ~RenderGaussianSplattingSystem() = default;

    void onExecute() override;

  private:
    void render(ecs::EntityId entity, WebContent &content) override;
    
    // Initialize material for 3DGS rendering
    bool initializeMaterial();
    
    // Get camera position for depth sorting
    glm::vec3 getCameraPosition();
    
    // Material for gaussian splatting
    std::shared_ptr<materials::GaussianSplattingMaterial> gaussianMaterial_;
    
    bool materialInitialized_ = false;
  };

  /**
   * System to render traditional 3D models (GLTF/GLB).
   * Placeholder for future GLTF rendering implementation.
   */
  class RenderGLTFSystem : public RenderBaseSystem
  {
  public:
    void onExecute() override;

  private:
    void render(ecs::EntityId entity, WebContent &content) override;
  };

  /**
   * System to update WebGL textures for 3D models.
   * Similar to UpdateTextureSystem in web_content_renderer.cpp
   */
  class UpdateTextureSystem : public RenderBaseSystem
  {
  public:
    void onExecute() override;

  private:
    void render(ecs::EntityId entity, WebContent &content) override;
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

      auto loadModel = System::Make<LoadModelSystem>();
      auto render3DGS = System::Make<RenderGaussianSplattingSystem>();
      auto renderGLTF = System::Make<RenderGLTFSystem>();
      auto updateTexture = System::Make<UpdateTextureSystem>();

      // Chain systems for proper execution order
      loadModel
        ->chain(render3DGS)
        ->chain(renderGLTF)
        ->chain(updateTexture);
      
      app.addSystem(SchedulerLabel::kUpdate, loadModel);
    }
  };
}