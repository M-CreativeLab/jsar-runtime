#pragma once

#include <string>
#include <chrono>
#include <glm/glm.hpp>

#include <common/xr/types.hpp>
#include <common/xr/message.hpp>
// TODO: move xr/render_state.hpp to common directory.
#include <xr/render_state.hpp>
#include "../dom/dom_event_target.hpp"

namespace client_xr
{
  static const char *LOG_TAG = "jsar.xr";

  using XRSessionMode = xr::TrXRSessionMode;
  using XRJointIndex = xr::TrXRJointIndex;
  using XRHandedness = xr::TrHandness;
  using XRTargetRayMode = xr::TrXRTargetRayMode;

  enum class XREnvironmentBlendMode
  {
    kOpaque = 0,
    kAdditive,
    kAlphaBlend,
  };

  inline std::string to_string(XREnvironmentBlendMode blendMode)
  {
    switch (blendMode)
    {
    case XREnvironmentBlendMode::kOpaque:
      return "opaque";
    case XREnvironmentBlendMode::kAdditive:
      return "additive";
    case XREnvironmentBlendMode::kAlphaBlend:
      return "alpha-blend";
    default:
      return "unknown";
    }
  }

  enum class XRSpaceSubType
  {
    kUnset = -1,
    kGrip = 0,
    kTargetRay,
  };

  inline std::string to_string(XRSpaceSubType spaceSubType)
  {
    switch (spaceSubType)
    {
    case XRSpaceSubType::kGrip:
      return "grip";
    case XRSpaceSubType::kTargetRay:
      return "targetRay";
    default:
      return "unset";
    }
  }

  /**
   * The type of the reference space in XR.
   *
   * The application developer can request different types of reference spaces to render the spatial content in different
   * ways. The reference space type is used to determine the origin of the coordinate system and the orientation of the
   * content.
   */
  enum class XRReferenceSpaceType
  {
    kViewer = 0,
    kLocal,
    kLocalFloor,
    kBoundedFloor,
    kUnbounded,
    kUnknown,
  };

  inline std::string to_string(XRReferenceSpaceType referenceSpaceType)
  {
    switch (referenceSpaceType)
    {
    case XRReferenceSpaceType::kViewer:
      return "viewer";
    case XRReferenceSpaceType::kLocal:
      return "local";
    case XRReferenceSpaceType::kLocalFloor:
      return "local-floor";
    case XRReferenceSpaceType::kBoundedFloor:
      return "bounded-floor";
    case XRReferenceSpaceType::kUnbounded:
      return "unbounded";
    default:
      return "unknown";
    }
  }

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
      init.requiredFeatures = {
          xr::TrXRFeature::LOCAL,
          xr::TrXRFeature::VIEWER};
      return init;
    }

  public:
    XRSessionRequestInit() {}

  public:
    /**
     * An array of values which the returned XRSession must support.
     */
    std::vector<xr::TrXRFeature> requiredFeatures;
    /**
     * An array of values identifying features which the returned XRSession may optionally support.
     */
    std::vector<xr::TrXRFeature> optionalFeatures;
    // TODO: more options
  };

  /**
   * The `XRSessionConfiguration` class represents the configuration of a new WebXR session. It is returned by the
   * XR device server after the request of a new WebXR session.
   */
  class XRSessionConfiguration
  {
  public:
    /**
     * Create a new `XRSessionConfiguration` instance.
     *
     * @param response The session response from the XR device server.
     * @param mode The session mode.
     * @param requestInit The initial options when requesting this session.
     * @param enabledFeatures The enabled features.
     */
    XRSessionConfiguration(xr::SessionResponse &response,
                           XRSessionMode mode,
                           XRSessionRequestInit requestInit,
                           std::vector<xr::TrXRFeature> enabledFeatures)
        : id(response.sessionId),
          recommendedContentSize(response.recommendedContentSize),
          mode(mode),
          requestInit(requestInit),
          enabledFeatures(enabledFeatures)
    {
    }

  public:
    int id;
    XRSessionMode mode;
    XRSessionRequestInit requestInit;
    float recommendedContentSize;
    std::vector<xr::TrXRFeature> enabledFeatures;
  };
}
