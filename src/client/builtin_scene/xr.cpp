#include <client/cssom/units.hpp>
#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>

#include "./ecs-inl.hpp"
#include "./hierarchy/element.hpp"
#include "./transform.hpp"
#include "./web_content.hpp"
#include "./scene.hpp"
#include "./xr.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_xr;

  WebXRExperience::WebXRExperience() : ecs::Resource(),
                                       client_(TrClientContextPerProcess::Get()->getXRDeviceClient()),
                                       session_(nullptr),
                                       reference_space_(nullptr),
                                       current_frame_(nullptr),
                                       current_time_(0)
  {
    assert(client_ != nullptr && "WebXRExperience: XRDeviceClient is null");
  }

  WebXRExperience::~WebXRExperience()
  {
    if (session_ != nullptr)
    {
      session_->removeEventListener(dom::DOMEventType::XRSessionSelectStart);
      session_->removeEventListener(dom::DOMEventType::XRSessionSelectEnd);
      session_->end();
      session_ = nullptr;
    }
  }

  bool WebXRExperience::multiviewRequired() const
  {
    assert(session_ != nullptr && "requestSession() must be called before.");
    return client_->getDeviceInit().multiviewRequired();
  }

  shared_ptr<XRSession> WebXRExperience::requestSession()
  {
    assert(session_ == nullptr && "WebXRExperience: Session already created");

    auto eventloop = TrClientContextPerProcess::Get()->getScriptingEventLoop();
    auto xrSystem = client_->getXRSystem(eventloop);
    session_ = xrSystem->requestSession();

    // Add event listeners for actions
    auto callback = [this](dom::DOMEventType type, shared_ptr<dom::Event> event)
    {
      if (type == dom::DOMEventType::XRSessionSelectStart)
      {
        if (select_start_handler_ == nullptr)
          return;
        select_start_handler_(*static_pointer_cast<XRInputSourceEvent>(event));
      }
      else if (type == dom::DOMEventType::XRSessionSelectEnd)
      {
        if (select_end_handler_ == nullptr)
          return;
        select_end_handler_(*static_pointer_cast<XRInputSourceEvent>(event));
      }
    };
    session_->addEventListener(dom::DOMEventType::XRSessionSelectStart, callback);
    session_->addEventListener(dom::DOMEventType::XRSessionSelectEnd, callback);
    return session_;
  }

  optional<collision::TrRay> WebXRExperience::selectRayForHitTesting()
  {
    if (!sessionRef().inputSources.has_value())
      return nullopt;

    for (auto &inputSource : sessionRef().inputSources.value())
    {
      if (inputSource->targetRayMode() == XRTargetRayMode::TrackedPointer)
      {
        shared_ptr<XRTargetRayOrGripSpace> raySpace = inputSource->targetRaySpace();
        XRRigidTransform rayTransform = current_frame_->getPose(raySpace, reference_space_)->transform;

        glm::mat4 rayBaseMatrix = rayTransform.matrix();
        glm::vec3 origin = glm::vec3(rayBaseMatrix[3]);
        glm::vec3 dir = glm::normalize(glm::vec3(rayBaseMatrix * glm::vec4(0.f, 0.f, -1.f, 0.f)));
        return make_optional(collision::TrRay(origin, dir, 100.f));
      }
    }
    return nullopt;
  }

  void WebXRExperienceStartupSystem::onExecute()
  {
  }

  void WebXRExperienceUpdateSystem::onExecute()
  {
    auto xrExperience = getResource<WebXRExperience>();
    assert(xrExperience != nullptr);

    auto xrFrame = xrExperience->currentFrame();
    auto referenceSpace = xrExperience->referenceSpace();
    if (xrFrame == nullptr || referenceSpace == nullptr)
      return;

    auto viewerPose = xrFrame->getViewerPose(referenceSpace);
    if (viewerPose == nullptr)
      return;
  }

  void WebXRCollisionBoxSystem::onExecute()
  {
    auto scene = getApplication<Scene>();
    assert(scene != nullptr);

    auto xrExperience = getResource<WebXRExperience>();
    if (xrExperience != nullptr)
    {
      float width = client_cssom::pixelToMeter(scene->volumeSize().width());
      float height = client_cssom::pixelToMeter(scene->volumeSize().height());
      glm::vec3 min(-width / 2, -height / 2, -0.001f);
      glm::vec3 max(width / 2, height / 2, 0.001f);
      xrExperience->sessionRef().updateCollisionBox(min, max);
    }
  }

  void WebXRPlugin::build(ecs::App &app)
  {
    using namespace ecs;

    // Resources
    app.addResource(Resource::Make<WebXRExperience>());

    // Systems
    app.addSystem(SchedulerLabel::kStartup, System::Make<WebXRExperienceStartupSystem>());
    app.addSystem(SchedulerLabel::kPreUpdate, System::Make<WebXRExperienceUpdateSystem>());
    app.addSystem(SchedulerLabel::kPostUpdate, System::Make<WebXRCollisionBoxSystem>());
  }
}
