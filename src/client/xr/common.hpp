#pragma once

#include <string>
#include <chrono>
#include <glm/glm.hpp>
#include "common/xr/types.hpp"
// TODO: move xr/render_state.hpp to common directory.
#include "xr/render_state.hpp"

namespace client_xr
{
  static const char *LOG_TAG = "jsar.xr";

  using XRSessionMode = xr::TrXRSessionMode;
  using XRRenderState = xr::RenderState;
  using XRJointIndex = xr::TrXRJointIndex;

  enum XREnvironmentBlendMode
  {
    kOpaque = 0,
    kAdditive,
    kAlphaBlend,
  };

  enum XRSpaceSubType
  {
    kUnset = -1,
    kGrip = 0,
    kTargetRay,
  };

  /**
   * The type of the reference space in XR.
   *
   * The application developer can request different types of reference spaces to render the spatial content in different
   * ways. The reference space type is used to determine the origin of the coordinate system and the orientation of the
   * content.
   */
  enum XRReferenceSpaceType
  {
    kViewer = 0,
    kLocal,
    kLocalFloor,
    kBoundedFloor,
    kUnbounded,
  };

  enum XREye
  {
    kLeft = 0,
    kRight = 1,
    kNone = 2,
  };
  using XRViewSpaceType = XREye;

  class XRDeviceClient;
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

  /**
   * The `XRSessionRequestInit` class represents the options for requesting a new WebXR session.
   *
   * see https://developer.mozilla.org/en-US/docs/Web/API/XRSystem/requestSession#options
   */
  class XRSessionRequestInit
  {
  public:
    XRSessionRequestInit() {}

  public:
    /**
     * An array of values which the returned XRSession must support.
     */
    std::vector<std::string> requiredFeatures;
    /**
     * An array of values identifying features which the returned XRSession may optionally support.
     */
    std::vector<std::string> optionalFeatures;
    // TODO: more options
  };

  /**
   * The `XRSessionConfiguration` class represents the configuration of a new WebXR session. It is returned by the
   * XR device server after the request of a new WebXR session.
   */
  class XRSessionConfiguration
  {
  public:
    XRSessionConfiguration(xr::SessionResponse &response)
        : id(response.id),
          recommendedContentSize(response.recommendedContentSize)
    {
    }

  public:
    int id;
    float recommendedContentSize;
  };

  // class XRFrameContext
  // {
  // public:
  //   XRFrameContext(xr::TrXRSessionContextData sessionContext, xr::TrXRDeviceContextData deviceContext, XRSession *session)
  //       : sessionId(sessionContext.sessionId),
  //         stereoId(sessionContext.stereoId),
  //         stereoTimestamp(sessionContext.timestampOnSettingStereoId),
  //         framebufferId(deviceContext.framebufferConf.id),
  //         framebufferWidth(deviceContext.framebufferConf.width),
  //         framebufferHeight(deviceContext.framebufferConf.height),
  //         session(session)
  //   {
  //     memcpy(localBaseMatrix, sessionContext.localBaseMatrix, sizeof(localBaseMatrix));
  //     memcpy(viewerBaseMatrix, deviceContext.stereoFrame.viewerBaseMatrix, sizeof(viewerBaseMatrix));
  //     views[0] = deviceContext.stereoFrame.views[0];
  //     views[1] = deviceContext.stereoFrame.views[1];

  //     auto now = chrono::system_clock::now();
  //     time = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
  //   }

  // public:
  //   uint32_t sessionId;
  //   uint32_t stereoId;
  //   long long stereoTimestamp;
  //   float localBaseMatrix[16];
  //   float viewerBaseMatrix[16];
  //   xr::TrXRView views[2];
  //   int framebufferId = -1;
  //   int framebufferWidth;
  //   int framebufferHeight;
  //   XRSession *session;
  //   uint32_t time = 0;
  // };

  // glm::mat4 createMat4FromArray(float *src);
}
