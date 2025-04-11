#include "./ecs-inl.hpp"
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

  void WebXRPlugin::build(ecs::App &app)
  {
    using namespace ecs;

    // Resources
    app.addResource(Resource::Make<WebXRExperience>());

    // Systems
    app.addSystem(SchedulerLabel::kStartup, System::Make<WebXRExperienceStartupSystem>());
    app.addSystem(SchedulerLabel::kPreUpdate, System::Make<WebXRExperienceUpdateSystem>());
  }
}
