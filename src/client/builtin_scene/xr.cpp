#include <client/dom/dom_event_target.hpp>
#include <client/xr/webxr_session_events.hpp>

#include "./ecs-inl.hpp"
#include "./transform.hpp"
#include "./hierarchy/element.hpp"
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
    glm::vec3 globalMin(std::numeric_limits<float>::max());
    glm::vec3 globalMax(std::numeric_limits<float>::lowest());

    const glm::vec3 localMin = glm::vec3(-1.0f, -1.0f, -1.0f);
    const glm::vec3 localMax = glm::vec3(1.0f, 1.0f, 1.0f);
    const std::vector<glm::vec3> localCorners = {
        glm::vec3(localMin.x, localMin.y, localMin.z),
        glm::vec3(localMin.x, localMin.y, localMax.z),
        glm::vec3(localMin.x, localMax.y, localMin.z),
        glm::vec3(localMin.x, localMax.y, localMax.z),
        glm::vec3(localMax.x, localMin.y, localMin.z),
        glm::vec3(localMax.x, localMin.y, localMax.z),
        glm::vec3(localMax.x, localMax.y, localMin.z),
        glm::vec3(localMax.x, localMax.y, localMax.z)};

    for (auto &entity : queryEntities<hierarchy::Element>())
    {
      auto &elementComponent = getComponentChecked<hierarchy::Element>(entity);
      auto transformComponent = getComponent<Transform>(entity);
      if (transformComponent == nullptr)
        continue;

      auto matrix = transformComponent->lastComputedMatrix();
      for (const glm::vec3& corner : localCorners)
      {
        glm::vec4 transformedCorner = matrix * glm::vec4(corner, 1.0f);
        glm::vec3 worldPos = glm::vec3(transformedCorner) / transformedCorner.w;

        globalMin = glm::min(globalMin, worldPos);
        globalMax = glm::max(globalMax, worldPos);
      }
    }

    cout << "Global Min: " << globalMin.x << ", " << globalMin.y << ", " << globalMin.z << endl;
    cout << "Global Max: " << globalMax.x << ", " << globalMax.y << ", " << globalMax.z << endl;
  }

  void WebXRPlugin::build(ecs::App &app)
  {
    using namespace ecs;

    // Resources
    app.addResource(Resource::Make<WebXRExperience>());

    // Systems
    app.addSystem(SchedulerLabel::kStartup, System::Make<WebXRExperienceStartupSystem>());
    app.addSystem(SchedulerLabel::kPreUpdate, System::Make<WebXRExperienceUpdateSystem>());
    // app.addSystem(SchedulerLabel::kPostUpdate, System::Make<WebXRCollisionBoxSystem>());
  }
}
