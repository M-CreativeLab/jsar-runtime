#pragma once

#include <glm/glm.hpp>
#include "./common.hpp"
#include "./webxr_rigid_transform.hpp"

namespace client_xr
{
  class XRPose
  {
  public:
    XRPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, glm::mat4 &transformationMatrix);
    XRPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, XRRigidTransform &transform);

  public:
    XRRigidTransform transform;
    bool emulatedPosition;

  protected:
    std::shared_ptr<XRSession> session_;
    std::shared_ptr<XRDeviceClient> device_;
    xr::TrXRFrameRequest *frameRequestData_;
  };

  class XRViewerPose : public XRPose
  {
  public:
    XRViewerPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, glm::mat4 &transformationMatrix, std::shared_ptr<XRReferenceSpace> baseReferenceSpace);
    XRViewerPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, XRRigidTransform &transform, std::shared_ptr<XRReferenceSpace> baseReferenceSpace);

  public:
    std::vector<std::shared_ptr<XRView>> &views()
    {
      return views_;
    }

  private:
    void setupViews(std::shared_ptr<XRReferenceSpace> baseReferenceSpace);

  private:
    std::vector<std::shared_ptr<XRView>> views_;
  };

  class XRJointPose : public XRPose
  {
  public:
    XRJointPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, glm::mat4 &transformationMatrix);
    XRJointPose(std::shared_ptr<XRSession> session, std::shared_ptr<XRFrame> frame, XRRigidTransform &transform);
  };
}
