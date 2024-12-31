#pragma once

#include <memory>
#include "./ecs-inl.hpp"
#include "./asset.hpp"
#include "./timer.hpp"
#include "./camera.hpp"
#include "./hierarchy.hpp"
#include "./transform.hpp"
#include "./meshes.hpp"
#include "./materials.hpp"
#include "./client_renderer.hpp"
#include "./xr.hpp"

#include "../graphics/webgl_context.hpp"
#include "../xr/device.hpp"
#include "../xr/webxr_session.hpp"
#include "../per_process.hpp"

namespace builtin_scene
{
  /**
   * The `DefaultPlugin` loads the default components and systems for the builtin scene.
   */
  class DefaultPlugin : public ecs::Plugin
  {
  public:
    using ecs::Plugin::Plugin;

  protected:
    void build(ecs::App &app) override
    {
      using namespace ecs;

      // Resources
      app.addResource(Resource::Make<Timer>(16));
      app.addResource(Resource::Make<Meshes>());
      app.addResource(Resource::Make<Materials>());

      // Components
      app.registerComponent<hierarchy::Element>();
      app.registerComponent<hierarchy::Children>();
      app.registerComponent<hierarchy::Parent>();
      app.registerComponent<Transform>();
      app.registerComponent<Camera>();
      app.registerComponent<Mesh3d>();
      app.registerComponent<MeshMaterial3d>();

      // Systems
      app.addSystem(SchedulerLabel::kStartup, System::Make<CameraStartupSystem>());
      app.addSystem(SchedulerLabel::kPreUpdate, System::Make<TimerSystem>());

      auto updateChain = System::Make<CameraUpdateSystem>()
                             ->chain(System::Make<RenderSystem>());
      app.addSystem(SchedulerLabel::kUpdate, updateChain);
    }
  };

  /**
   * The main class for the builtin scene which inherits from the `ecs::App`.
   */
  class Scene : public ecs::App
  {
  public:
    /**
     * Create a new instance of the Scene.
     *
     * @param clientContext The client context to use.
     * @returns The new instance of the Scene.
     */
    static std::shared_ptr<Scene> Make(TrClientContextPerProcess *clientContext)
    {
      if (TR_UNLIKELY(clientContext == nullptr))
        clientContext = TrClientContextPerProcess::Get();
      assert(clientContext != nullptr);
      return std::make_shared<Scene>(clientContext);
    }

  public:
    Scene(TrClientContextPerProcess *clientContext)
        : ecs::App(),
          glContext_(clientContext->createHostWebGLContext()),
          xrDeviceClient_(clientContext->getXRDeviceClient())
    {
      assert(glContext_ != nullptr);
      assert(xrDeviceClient_ != nullptr);

      frameCallback_ = [this](uint32_t time, std::shared_ptr<client_xr::XRFrame> frame, void *env_)
      {
        assert(xrSession_ != nullptr); // ensure the WebXR session is ready.
        update(time, frame);
        xrSession_->requestAnimationFrame(frameCallback_);
      };

      auto setupXRSession = [this, clientContext](auto type, auto event)
      {
        auto xrSystem = xrDeviceClient_->getXRSystem(clientContext->getScriptingEventLoop());
        xrSession_ = xrSystem->requestSession();

        // Update the render state
        client_xr::XRRenderState newRenderState;
        newRenderState.baseLayer = client_xr::XRWebGLLayer::Make(xrSession_, glContext_);
        xrSession_->updateRenderState(newRenderState);

        bootstrap();
        xrSession_->requestAnimationFrame(frameCallback_);
      };
      clientContext->addEventListener(TrClientContextEventType::ScriptingEventLoopReady, setupXRSession);
    }
    ~Scene() = default;

  public:
    /**
     * Create a new element to the scene for rendering.
     *
     * @param name The tag name of the element.
     */
    ecs::EntityId createElementEntity(std::string name)
    {
      return spawn(
          hierarchy::Element(name),
          hierarchy::Children(),
          hierarchy::Parent(),
          Transform());
    }

  private:
    void bootstrap()
    {
      addPlugin<DefaultPlugin>();
      addPlugin<WebXRPlugin>();
      ecs::App::startup();

      // Add renderer
      addResource(ecs::Resource::Make<Renderer>(glContext_));

      // Update WebXR session resource
      {
        auto xrExperience = getResource<WebXRExperience>();
        assert(xrExperience != nullptr);
        xrExperience->updateReferenceSpace(
            xrSession_->requestReferenceSpace(client_xr::XRReferenceSpaceType::kLocal));
      }

      // Get the meshes and materials resources
      auto meshes = getResource<Meshes>();
      auto materials = getResource<Materials>();

      // Spawn the camera entity
      spawn(Camera(), Transform());

      // Spawn the default mesh
      // auto colorMaterial = Material::Make<materials::ColorMaterial>(1.0f, 0.0f, 0.0f);
      // spawn(
      //     Mesh3d(meshes->add(MeshBuilder::CreateBox(0.25f, 0.20f, 0.05f))),
      //     MeshMaterial3d(materials->add(colorMaterial)),
      //     Transform());
    }
    void update(uint32_t time, std::shared_ptr<client_xr::XRFrame> frame)
    {
      // Update the time and frame to the WebXRSession resource.
      auto xrExperience = getResource<WebXRExperience>();
      assert(xrExperience != nullptr);
      xrExperience->updateCurrentFrame(time, frame);

      // Trigger the ECS update().
      ecs::App::update();
    }

  private:
    std::shared_ptr<client_graphics::WebGL2Context> glContext_;
    std::shared_ptr<client_xr::XRDeviceClient> xrDeviceClient_;
    std::shared_ptr<client_xr::XRSession> xrSession_;
    client_xr::XRFrameCallback frameCallback_;
  };
}
