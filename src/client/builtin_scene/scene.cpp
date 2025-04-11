#include <client/dom/element.hpp>

#include "./scene.hpp"
#include "./client_renderer.hpp"

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
      app.registerComponent<BoundingBox>();
      app.registerComponent<Camera>();
      app.registerComponent<Mesh3d>();
      app.registerComponent<MeshMaterial3d>();
      app.registerComponent<Text2d>();
      app.registerComponent<Image2d>();

      // Systems
      app.addSystem(SchedulerLabel::kStartup, System::Make<CameraStartupSystem>());
      app.addSystem(SchedulerLabel::kStartup, System::Make<RenderStartupSystem>());
      app.addSystem(SchedulerLabel::kPreUpdate, System::Make<TimerSystem>());

      auto updateCamera = System::Make<CameraUpdateSystem>();
      auto renderScene = System::Make<RenderSystem>();
      updateCamera->chain(renderScene);
      app.addSystem(SchedulerLabel::kUpdate, updateCamera);
    }
  };

  Scene::Scene(TrClientContextPerProcess *clientContext)
      : ecs::App(),
        clientContext_(clientContext),
        glContext_(clientContext->createHostWebGLContext())
  {
    assert(glContext_ != nullptr);

    frameCallback_ = [this](uint32_t time, shared_ptr<client_xr::XRFrame> frame, void *env_)
    {
      if (paused_)
        return;
      assert(xrSession_ != nullptr); // ensure the WebXR session is ready.
      update(time, frame);
      xrSession_->requestAnimationFrame(frameCallback_);
    };

    addPlugin<DefaultPlugin>();
    addPlugin<WebContentPlugin>();
    addPlugin<WebXRPlugin>();
    addResource(ecs::Resource::Make<Renderer>(glContext_, volumeSize_));
  }

  void Scene::bootstrap()
  {
    ecs::App::startup();
  }

  void Scene::start(optional<math::Size3> volumeSize)
  {
    if (started_)
    {
      resume();
    }
    else
    {
      started_ = true;
      setVolumeSize(volumeSize);

      if (clientContext_->isScriptingEventLoopReady())
      {
        setupXRSession();
      }
      else
      {
        clientContext_->addEventListener(
            TrClientContextEventType::ScriptingEventLoopReady, [this](auto _type, auto _event)
            { setupXRSession(); });
      }
    }
  }

  void Scene::pause()
  {
    paused_ = true;
  }

  void Scene::resume()
  {
    if (!started_)
      throw runtime_error("Scene is not started yet");
    paused_ = false;
    xrSession_->requestAnimationFrame(frameCallback_);
  }

  ecs::EntityId Scene::createElement(string name, shared_ptr<dom::Node> node,
                                     optional<ecs::EntityId> parent)
  {
    Transform defaultTransform = Transform::FromXYZ(0.0f, 0.0f, 0.0f);
    BoundingBox defaultBoundingBox = BoundingBox();

    if (!parent.has_value())
    {
      bool isRootRenderable = false;
      auto elementNode = dynamic_pointer_cast<dom::Element>(node);
      if (elementNode != nullptr && elementNode->is("body"))
        isRootRenderable = true; // Only the body element as Root is renderable by default.
      return spawn(
          hierarchy::Element(name, node),
          hierarchy::Children(),
          hierarchy::Root(isRootRenderable),
          BoundingBox(),
          defaultTransform);
    }
    else
    {
      ecs::EntityId rootEntity;
      ecs::EntityId parentEntity = parent.value();
      auto parentRoot = getComponent<hierarchy::Root>(parentEntity);
      if (parentRoot != nullptr)
      {
        rootEntity = parentEntity;
      }
      else
      {
        auto parentParent = getComponent<hierarchy::Parent>(parentEntity);
        assert(parentParent != nullptr && "Parent entity must have a parent component");
        rootEntity = parentParent->root();
      }

      auto newEntity = spawn(
          hierarchy::Element(name, node),
          hierarchy::Children(),
          hierarchy::Parent(parentEntity, rootEntity),
          BoundingBox(),
          defaultTransform);

      // Update the parent's children
      auto children = getComponent<hierarchy::Children>(parentEntity);
      if (children != nullptr)
        children->addChild(newEntity);
      return newEntity;
    }
  }

  bool Scene::removeElement(ecs::EntityId entity)
  {
    return removeEntity(entity);
  }

  std::shared_ptr<WebXRExperience> Scene::getWebXRExperience()
  {
    return getResource<WebXRExperience>();
  }

  optional<collision::TrRay> Scene::selectRayForHitTesting()
  {
    auto xrExperience = getResource<WebXRExperience>();
    assert(xrExperience != nullptr);
    return xrExperience->selectRayForHitTesting();
  }

  void Scene::onSelectStart(SelectEventHandler handler)
  {
    getResource<WebXRExperience>()->resetSelectStartHandler(handler);
  }

  void Scene::onSelectEnd(SelectEventHandler handler)
  {
    getResource<WebXRExperience>()->resetSelectEndHandler(handler);
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

  void Scene::setupXRSession()
  {
    shared_ptr<WebXRExperience> xrExperience = getResource<WebXRExperience>();
    assert(xrExperience != nullptr);
    xrSession_ = xrExperience->requestSession();

    // Update the render state
    client_xr::XRRenderState newRenderState;
    newRenderState.baseLayer = client_xr::XRWebGLLayer::Make(xrSession_, glContext_);
    xrSession_->updateRenderState(newRenderState);

    // Initialize the WebXR experience
    {
      xrExperience->updateReferenceSpace(
          xrSession_->requestReferenceSpace(client_xr::XRReferenceSpaceType::kLocal));

      // Update the multiview flag if required
      if (xrExperience->multiviewRequired())
      {
        xrExperience->enableMultiview(true);
        Material::SetGlobalDefines("MULTIVIEW");
        Material::SetMultiviewRequired(true);
      }
    }
    resume();
  }
}
