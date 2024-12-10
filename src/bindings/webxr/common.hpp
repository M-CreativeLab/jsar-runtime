#pragma once

#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include "debug.hpp"
#include "common/xr/types.hpp"

namespace bindings
{
  using namespace std;
  using namespace glm;

  static const char *LOG_TAG = "jsar.xr";

  enum XRSpaceSubType
  {
    GRIP = 0,
    TARGET_RAY = 1,
    UNSET = -1,
  };

  enum XRReferenceSpaceType
  {
    VIEWER = 0,
    LOCAL = 1,
    LOCAL_FLOOR = 2,
    BOUNDED_FLOOR = 3,
    UNBOUNDED = 4,
  };

  enum XREye
  {
    LEFT = 0,
    RIGHT = 1,
    NONE = 2,
  };
  using XRViewSpaceType = XREye;

  class XRDeviceNative;
  class XRSession;
  class XRFrame;
  class XRSpace;
  class XRReferenceSpace;
  class XRViewSpace;
  class XRTargetRayOrGripSpace;
  class XRPose;
  class XRViewerPose;
  class XRHand;
  class XRInputSource;
  class XRInputSourceArray;

  class XRFrameContext
  {
  public:
    XRFrameContext(xr::TrXRSessionContextData sessionContext, xr::TrXRDeviceContextData deviceContext, XRSession *session)
        : sessionId(sessionContext.sessionId),
          stereoId(sessionContext.stereoId),
          stereoTimestamp(sessionContext.timestampOnSettingStereoId),
          framebufferId(deviceContext.framebufferConf.id),
          framebufferWidth(deviceContext.framebufferConf.width),
          framebufferHeight(deviceContext.framebufferConf.height),
          session(session)
    {
      memcpy(localBaseMatrix, sessionContext.localBaseMatrix, sizeof(localBaseMatrix));
      memcpy(viewerBaseMatrix, deviceContext.stereoFrame.viewerBaseMatrix, sizeof(viewerBaseMatrix));
      views[0] = deviceContext.stereoFrame.views[0];
      views[1] = deviceContext.stereoFrame.views[1];

      auto now = chrono::system_clock::now();
      time = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
    }

  public:
    uint32_t sessionId;
    uint32_t stereoId;
    long long stereoTimestamp;
    float localBaseMatrix[16];
    float viewerBaseMatrix[16];
    xr::TrXRView views[2];
    int framebufferId = -1;
    int framebufferWidth;
    int framebufferHeight;
    XRSession *session;
    uint32_t time = 0;
  };

  glm::mat4 createMat4FromArray(float *src);
}
