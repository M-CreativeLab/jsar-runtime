#pragma once

#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include <common/xr/types.hpp>
#include <common/xr/message.hpp>
// TODO: move xr/render_state.hpp to common directory.
#include <xr/render_state.hpp>

namespace client_xr
{
  static const char *LOG_TAG = "jsar.xr";

  using XRSessionMode = xr::TrXRSessionMode;
  using XRJointIndex = xr::TrXRJointIndex;
  using XRHandedness = xr::TrHandness;
  using XRTargetRayMode = xr::TrXRTargetRayMode;

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
  class XRSystem;
  class XRSession;
  class XRFrame;
  class XRSpace;
  class XRReferenceSpace;
  class XRJointSpace;
  class XRViewSpace;
  class XRTargetRayOrGripSpace;
  class XRPose;
  class XRViewerPose;
  class XRJointPose;
  class XRView;
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
    static XRSessionRequestInit Default()
    {
      XRSessionRequestInit init;
      init.requiredFeatures = {"viewer"};
      return init;
    }

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
    XRSessionConfiguration(xr::SessionResponse &response, XRSessionMode mode, XRSessionRequestInit requestInit)
        : id(response.id),
          recommendedContentSize(response.recommendedContentSize),
          mode(mode),
          requestInit(requestInit)
    {
    }

  public:
    int id;
    XRSessionMode mode;
    XRSessionRequestInit requestInit;
    float recommendedContentSize;
  };
}
