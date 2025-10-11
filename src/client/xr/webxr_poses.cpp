#include "./device.hpp"
#include "./webxr_poses.hpp"
#include "./webxr_view.hpp"
#include "./webxr_session.hpp"
#include "./webxr_frame.hpp"

using namespace std;

namespace client_xr
{
  XRPose::XRPose(shared_ptr<XRSession> session, shared_ptr<XRFrame> frame, glm::mat4 &transformationMatrix)
      : session_(session)
      , device_(session->device())
      , transform(transformationMatrix)
      , emulatedPosition(false)
  {
  }

  XRPose::XRPose(shared_ptr<XRSession> session, shared_ptr<XRFrame> frame, XRRigidTransform &transform)
      : session_(session)
      , device_(session->device())
      , transform(transform)
      , emulatedPosition(false)
  {
  }

  XRViewerPose::XRViewerPose(shared_ptr<XRSession> session,
                             shared_ptr<XRFrame> frame,
                             glm::mat4 &transformationMatrix,
                             shared_ptr<XRReferenceSpace> baseReferenceSpace)
      : XRPose(session, frame, transformationMatrix)
  {
    setupViews(frame, baseReferenceSpace);
  }
  XRViewerPose::XRViewerPose(shared_ptr<XRSession> session,
                             shared_ptr<XRFrame> frame,
                             XRRigidTransform &transform,
                             shared_ptr<XRReferenceSpace> baseReferenceSpace)
      : XRPose(session, frame, transform)
  {
    setupViews(frame, baseReferenceSpace);
  }

  void XRViewerPose::setupViews(shared_ptr<XRFrame> frame,
                                shared_ptr<XRReferenceSpace> baseReferenceSpace)
  {
    // Use the frame request data from the frame
    const xr::TrXRFrameRequest &frameRequestData = frame->frameRequestData_;

    /**
     * Create views.
     *
     * If the device is rendered as multipass, only one view is created, and the view is for the current eye.
     * Otherwise, 2 views are created, one for each eye.
     */
    auto device = session_->device();
    if (device->getDeviceInit().renderedAsMultipass())
    {
      auto &viewData = frameRequestData.views[frameRequestData.viewIndex];
      views_.push_back(make_shared<XRView>(viewData, session_, baseReferenceSpace));
    }
    else
    {
      size_t usedViewsCount = device->getDeviceInit().usedViewsCount;
      for (size_t viewIndex = 0; viewIndex < usedViewsCount; viewIndex++)
      {
        const auto &viewData = frameRequestData.views[viewIndex];
        views_.push_back(make_shared<XRView>(viewData, session_, baseReferenceSpace));
      }
    }
  }

  XRJointPose::XRJointPose(shared_ptr<XRSession> session, shared_ptr<XRFrame> frame, glm::mat4 &transformationMatrix)
      : XRPose(session, frame, transformationMatrix)
  {
  }

  XRJointPose::XRJointPose(shared_ptr<XRSession> session, shared_ptr<XRFrame> frame, XRRigidTransform &transform)
      : XRPose(session, frame, transform)
  {
  }
}
