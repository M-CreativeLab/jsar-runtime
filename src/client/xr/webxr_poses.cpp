#include "./device.hpp"
#include "./webxr_poses.hpp"
#include "./webxr_view.hpp"
#include "./webxr_session.hpp"

namespace client_xr
{
  XRPose::XRPose(std::shared_ptr<XRSession> session, glm::mat4 &transformationMatrix)
      : session_(session),
        transform(transformationMatrix),
        emulatedPosition(false)
  {
  }

  XRPose::XRPose(std::shared_ptr<XRSession> session, XRRigidTransform &transform)
      : session_(session),
        transform(transform),
        emulatedPosition(false)
  {
  }

  XRViewerPose::XRViewerPose(std::shared_ptr<XRSession> session,
                             glm::mat4 &transformationMatrix,
                             std::shared_ptr<XRReferenceSpace> baseReferenceSpace)
      : XRPose(session, transformationMatrix)
  {
    setupViews(baseReferenceSpace);
  }
  XRViewerPose::XRViewerPose(std::shared_ptr<XRSession> session,
                             XRRigidTransform &transform,
                             std::shared_ptr<XRReferenceSpace> baseReferenceSpace)
      : XRPose(session, transform)
  {
    setupViews(baseReferenceSpace);
  }

  void XRViewerPose::setupViews(std::shared_ptr<XRReferenceSpace> baseReferenceSpace)
  {
    /**
     * Create views.
     *
     * If the device is rendered as multipass, only one view is created, and the view is for the current eye.
     * Otherwise, 2 views are created, one for each eye.
     */
    auto device = session_->device();
    if (device->getDeviceInit().renderedAsMultipass())
    {
      auto &viewData = frameRequestData_->views[frameRequestData_->viewIndex];
      views_.push_back(std::make_shared<XRView>(viewData, session_, baseReferenceSpace));
    }
    else
    {
      for (size_t viewIndex = 0; viewIndex < xr::TrXRFrameRequest::ViewsCount; viewIndex++)
      {
        auto &viewData = frameRequestData_->views[viewIndex];
        views_.push_back(std::make_shared<XRView>(viewData, session_, baseReferenceSpace));
      }
    }
  }

  XRJointPose::XRJointPose(std::shared_ptr<XRSession> session, glm::mat4 &transformationMatrix)
      : XRPose(session, transformationMatrix)
  {
  }

  XRJointPose::XRJointPose(std::shared_ptr<XRSession> session, XRRigidTransform &transform)
      : XRPose(session, transform)
  {
  }
}
