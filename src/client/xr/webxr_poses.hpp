#pragma once

#include <glm/glm.hpp>
#include "./common.hpp"
#include "./webxr_rigid_transform.hpp"
#include "./webxr_view.hpp"

namespace client_xr
{
  class XRPose
  {
  public:
    XRPose(glm::mat4 &transformationMatrix, bool emulatedPosition = false)
        : transform(transformationMatrix),
          emulatedPosition(emulatedPosition)
    {
    }
    XRPose(XRRigidTransform &transform, bool emulatedPosition = false)
        : transform(transform),
          emulatedPosition(emulatedPosition)
    {
    }

  public:
    XRRigidTransform transform;
    bool emulatedPosition;

  protected:
    std::shared_ptr<XRDeviceClient> device_;
    xr::TrXRFrameRequest *frameData_;
  };

  class XRViewerPose : public XRPose
  {
  public:
    XRViewerPose(glm::mat4 &transformationMatrix, bool emulatedPosition = false)
        : XRPose(transformationMatrix, emulatedPosition)
    {
    }
    XRViewerPose(XRRigidTransform &transform, bool emulatedPosition = false)
        : XRPose(transform, emulatedPosition)
    {
    }

  public:
    std::vector<XRView> &views();

  private:
    std::vector<XRView> views_;
  };
}
