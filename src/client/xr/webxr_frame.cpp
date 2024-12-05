#include "./webxr_frame.hpp"
#include "./webxr_session.hpp"
#include "./webxr_spaces.hpp"
#include "./device.hpp"

using namespace std;
using namespace std::chrono;

namespace client_xr
{
  XRFrame::XRFrame(xr::TrXRFrameRequest *frameRequest, std::shared_ptr<XRSession> session)
      : frameRequestData_(frameRequest),
        id_(frameRequest->id),
        stereoId_(frameRequest->stereoId),
        timestamp_(frameRequest->time),
        session_(session),
        sessionId_(session->id),
        device_(session->device()),
        active_(false),
        animationFrame_(false)
  {
  }

  void XRFrame::startFrame()
  {
    active_ = true;
    animationFrame_ = true;
    bool isNewStereoFrame = false;
    auto isMultipass = device_->getDeviceInit().renderedAsMultipass();
    if (
        !isMultipass ||                   /** SinglePass */
        frameRequestData_->viewIndex == 0 /** MultiPass's right view */
    )
      isNewStereoFrame = true;

    device_->startFrame(frameRequestData_);
    session_->updateFrameTime(isNewStereoFrame);
    startTime_ = session_->frameTime();
  }

  void XRFrame::endFrame()
  {
    active_ = false;
    device_->endFrame(frameRequestData_);
    endTime_ = steady_clock::now();

    auto isMultipass = device_->getDeviceInit().renderedAsMultipass();
    auto frameDuration = duration_cast<microseconds>(endTime_ - startTime_).count() / 1000.0;

    // Calculate the fps threshold and log if the frame takes too long
    int threshold = 1000 / 45;
    if (isMultipass)
      threshold /= 2;
    if (frameDuration > threshold)
    {
      auto viewIndex = frameRequestData_->viewIndex;
      std::cerr << "Detected a long frame(#" << id() << ") at session(" << sessionId_ << ")'s view(" << viewIndex << ")";
      std::cerr << " takes " << frameDuration << "ms > " << threshold << "ms" << std::endl;
    }

    if (!isMultipass || frameRequestData_->viewIndex == 1)
    {
      // Calculate the Fps and update to fs on the right view
      auto &perfFs = device_->clientContext()->getPerfFs();
      if (session_->calcFps())
        perfFs.setFps(session_->fps_);
    }
  }

  std::shared_ptr<XRPose> XRFrame::getPose(std::shared_ptr<XRSpace> space, std::shared_ptr<XRSpace> baseSpace)
  {
    if (space == nullptr || baseSpace == nullptr)
      return nullptr;

    baseSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);
    if (!space->isReferenceSpace() && space->subType != XRSpaceSubType::kUnset)
    {
      auto inputSpace = dynamic_pointer_cast<XRTargetRayOrGripSpace>(space);
      inputSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);
      auto transform /** input source space to base(local/unbound) */ = XRSPACE_RELATIVE_TRANSFORM(inputSpace, baseSpace);
      return std::make_shared<XRPose>(session_, transform);
    }
    else
    {
      return nullptr;
    }
  }

  std::shared_ptr<XRViewerPose> XRFrame::getViewerPose(std::shared_ptr<XRReferenceSpace> referenceSpace)
  {
    if (referenceSpace == nullptr)
      return nullptr;

    auto viewerSpace = session_->viewerSpace();
    assert(viewerSpace != nullptr);

    referenceSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);
    viewerSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);

    auto viewerTransform /** viewer space to reference space */ = XRSPACE_RELATIVE_TRANSFORM(viewerSpace, referenceSpace);
    return std::make_shared<XRViewerPose>(session_, viewerTransform, referenceSpace);
  }

  std::shared_ptr<XRJointPose> XRFrame::getJointPose(std::shared_ptr<XRJointSpace> jointSpace, std::shared_ptr<XRSpace> baseSpace)
  {
    if (jointSpace == nullptr || baseSpace == nullptr)
      return nullptr;

    jointSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);
    if (baseSpace->isReferenceSpace())
    {
      baseSpace->ensurePoseUpdated(id_, session_, *frameRequestData_);
      auto transform /** joint space to base(local/unbound) */ = XRSPACE_RELATIVE_TRANSFORM(jointSpace, baseSpace);
      return std::make_shared<XRJointPose>(session_, transform);
    }
    else
    {
      return nullptr;
    }
  }
}
