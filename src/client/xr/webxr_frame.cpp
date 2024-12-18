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

  XRFrame::XRFrame(XRFrame &other)
      : frameRequestData_(other.frameRequestData_),
        id_(other.id_),
        stereoId_(other.stereoId_),
        timestamp_(other.timestamp_),
        session_(other.session_),
        sessionId_(other.sessionId_),
        device_(other.device_),
        active_(other.active_),
        animationFrame_(other.animationFrame_)
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
    if (space == nullptr)
      throw std::invalid_argument("`space` parameter is required");
    if (baseSpace == nullptr)
      throw std::invalid_argument("`baseSpace` parameter is required");

    auto &frameRequestData = *frameRequestData_;
    baseSpace->ensurePoseUpdated(id_, session_, frameRequestData);
    if (!space->isReferenceSpace() && space->subType != XRSpaceSubType::kUnset)
    {
      auto inputSpace = dynamic_pointer_cast<XRTargetRayOrGripSpace>(space);
      inputSpace->ensurePoseUpdated(id_, session_, frameRequestData);
      auto transform /** input source space to base(local/unbound) */ = TR_XRSPACE_RELATIVE_TRANSFORM(inputSpace, baseSpace);
      return std::make_shared<XRPose>(session_, shared_from_this(), transform);
    }
    else
    {
      throw std::invalid_argument("`space` parameter is not supported");
    }
  }

  std::shared_ptr<XRViewerPose> XRFrame::getViewerPose(std::shared_ptr<XRReferenceSpace> referenceSpace)
  {
    if (referenceSpace == nullptr)
      return nullptr;

    auto viewerSpace = session_->viewerSpace();
    assert(viewerSpace != nullptr);

    auto &frameRequestData = *frameRequestData_;
    referenceSpace->ensurePoseUpdated(id_, session_, frameRequestData);
    viewerSpace->ensurePoseUpdated(id_, session_, frameRequestData);

    auto viewerTransform /** viewer space to reference space */ = TR_XRSPACE_RELATIVE_TRANSFORM(viewerSpace, referenceSpace);
    return std::make_shared<XRViewerPose>(session_, shared_from_this(), viewerTransform, referenceSpace);
  }

  std::shared_ptr<XRJointPose> XRFrame::getJointPose(std::shared_ptr<XRJointSpace> jointSpace, std::shared_ptr<XRSpace> baseSpace)
  {
    if (jointSpace == nullptr || baseSpace == nullptr)
      return nullptr;

    auto &frameRequestData = *frameRequestData_;
    jointSpace->ensurePoseUpdated(id_, session_, frameRequestData);
    if (baseSpace->isReferenceSpace())
    {
      baseSpace->ensurePoseUpdated(id_, session_, frameRequestData);
      auto transform /** joint space to base(local/unbound) */ = TR_XRSPACE_RELATIVE_TRANSFORM(jointSpace, baseSpace);
      return std::make_shared<XRJointPose>(session_, shared_from_this(), transform);
    }
    else
    {
      return nullptr;
    }
  }
}
