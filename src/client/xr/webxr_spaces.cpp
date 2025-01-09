#include "./webxr_spaces.hpp"
#include "./webxr_input_sources.hpp"

namespace client_xr
{
  using namespace std;
  using namespace glm;

  void XRReferenceSpace::onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
  {
    if (type_ == XRReferenceSpaceType::kViewer)
    {
      baseMatrix_ = make_mat4(frameRequest.viewerBaseMatrix);
      XRSpace::onPoseUpdate(session, frameRequest);
    }
    else if (type_ == XRReferenceSpaceType::kLocal)
    {
      baseMatrix_ = make_mat4(frameRequest.localBaseMatrix);
      XRSpace::onPoseUpdate(session, frameRequest);
    }
    else if (type_ == XRReferenceSpaceType::kUnbounded)
    {
      baseMatrix_ = make_mat4(frameRequest.localBaseMatrix) * math::GetOriginMatrix();
      XRSpace::onPoseUpdate(session, frameRequest);
    }
  }

  void XRViewSpace::onPoseUpdate(std::shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
  {
    if (type_ == XRViewSpaceType::kNone)
      return; // No need to update the pose if the type is none.

    // TODO: check if a device
    auto &view = frameRequest.views[frameRequest.viewIndex];
    if (view.viewIndex != frameRequest.viewIndex)
    {
      std::cerr << "failed to update pose for XRViewSpace: viewIndex mismatch" << std::endl;
      return;
    }
    baseMatrix_ = inverse(view.getViewMatrix());
    projectionMatrix_ = view.getProjectionMatrix();
    XRSpace::onPoseUpdate(session, frameRequest);
  }

  void XRJointSpace::onPoseUpdate(shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
  {
    auto inputSourceData = inputSource->inputSourceData_;
    baseMatrix_ = make_mat4(inputSourceData->joints[static_cast<int>(index)].baseMatrix);
    XRSpace::onPoseUpdate(session, frameRequest);
  }

  void XRTargetRayOrGripSpace::onPoseUpdate(shared_ptr<XRSession> session, xr::TrXRFrameRequest &frameRequest)
  {
    if (subType == XRSpaceSubType::kGrip)
      baseMatrix_ = make_mat4(inputSource->inputSourceData_->gripBaseMatrix);
    else if (subType == XRSpaceSubType::kTargetRay)
      baseMatrix_ = make_mat4(inputSource->inputSourceData_->targetRayBaseMatrix);
    XRSpace::onPoseUpdate(session, frameRequest);
  }
}
