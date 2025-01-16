#include "./scene.hpp"

namespace builtin_scene
{
  using namespace std;

  /**
   * The `DefaultPlugin` loads the default components and systems for the builtin scene.
   */
  class DefaultPlugin final : public ecs::Plugin
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
      app.registerComponent<hierarchy::Root>();
      app.registerComponent<Transform>();
      app.registerComponent<Camera>();
      app.registerComponent<Mesh3d>();
      app.registerComponent<MeshMaterial3d>();
      app.registerComponent<Text2d>();

      // Systems
      app.addSystem(SchedulerLabel::kStartup, System::Make<CameraStartupSystem>());
      app.addSystem(SchedulerLabel::kPreUpdate, System::Make<TimerSystem>());

      auto updateChain = System::Make<CameraUpdateSystem>()
                             ->chain(System::Make<RenderSystem>());
      app.addSystem(SchedulerLabel::kUpdate, updateChain);
    }
  };

  Scene::Scene(TrClientContextPerProcess *clientContext)
      : ecs::App(),
        glContext_(clientContext->createHostWebGLContext()),
        xrDeviceClient_(clientContext->getXRDeviceClient())
  {
    assert(glContext_ != nullptr);
    assert(xrDeviceClient_ != nullptr);

    frameCallback_ = [this](uint32_t time, shared_ptr<client_xr::XRFrame> frame, void *env_)
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

  ecs::EntityId Scene::createElement(string name, std::optional<ecs::EntityId> parent)
  {
    Transform defaultTransform = Transform::FromXYZ(0.0f, 0.0f, 0.0f);
    if (!parent.has_value())
    {
      return spawn(
          hierarchy::Element(name),
          hierarchy::Children(),
          hierarchy::Root(),
          defaultTransform);
    }
    else
    {
      auto parentEntity = parent.value();
      auto newEntity = spawn(
          hierarchy::Element(name),
          hierarchy::Children(),
          hierarchy::Parent(parentEntity),
          defaultTransform);

      // Update the parent's children
      auto children = getComponent<hierarchy::Children>(parentEntity);
      if (children != nullptr)
        children->addChild(newEntity);
      return newEntity;
    }
  }

  void Scene::bootstrap()
  {
    addPlugin<DefaultPlugin>();
    addPlugin<WebContentPlugin>();
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
  }

  void Scene::update(uint32_t time, shared_ptr<client_xr::XRFrame> frame)
  {
    // Update the time and frame to the WebXRSession resource.
    auto xrExperience = getResource<WebXRExperience>();
    assert(xrExperience != nullptr);
    xrExperience->updateCurrentFrame(time, frame);

    // Trigger the ECS update().
    ecs::App::update();
  }
}
