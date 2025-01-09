#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <glm/glm.hpp>

#include <common/xr/types.hpp>
#include <common/xr/message.hpp>
// TODO: move xr/render_state.hpp to common directory.
#include <xr/render_state.hpp>
#include "../dom/dom_event_target.hpp"

namespace client_xr
{
  static constexpr const char *LOG_TAG = "jsar.xr";

  using XRSessionMode = xr::TrXRSessionMode;
  using XRJointIndex = xr::TrXRJointIndex;
  using XRHandedness = xr::TrHandness;
  using XRTargetRayMode = xr::TrXRTargetRayMode;

  /**
   * @enum XREnvironmentBlendMode
   * The `XREnvironmentBlendMode` enum represents the environment blend mode in XR.
   */
  enum class XREnvironmentBlendMode
  {
    kOpaque = 0, // Opaque environment blend mode
    kAdditive,   // Additive environment blend mode
    kAlphaBlend, // Alpha blend environment blend mode
  };

  /**
   * Converts the environment blend mode to a string.
   *
   * @param blendMode The environment blend mode.
   * @returns The string representation of the environment blend mode.
   */
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

  /**
   * @enum XRSpaceSubType
   * The `XRSpaceSubType` enum represents the subtype of an XR space.
   */
  enum class XRSpaceSubType
  {
    kUnset = -1, // Unset space subtype
    kGrip = 0,   // Grip space subtype
    kTargetRay,  // Target ray space subtype
  };

  /**
   * Converts the space subtype to a string.
   *
   * @param spaceSubType The space subtype.
   * @returns The string representation of the space subtype.
   */
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
   * @enum XRReferenceSpaceType
   * The `XRReferenceSpaceType` enum represents the type of the reference space in XR.
   */
  enum class XRReferenceSpaceType
  {
    kViewer = 0,   // Viewer reference space
    kLocal,        // Local reference space
    kLocalFloor,   // Local floor reference space
    kBoundedFloor, // Bounded floor reference space
    kUnbounded,    // Unbounded reference space
    kUnknown,      // Unknown reference space
  };

  /**
   * Converts the reference space type to a string.
   *
   * @param referenceSpaceType The reference space type.
   * @returns The string representation of the reference space type.
   */
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

  /**
   * @enum XREye
   * The `XREye` enum represents the eye in XR.
   */
  enum class XREye
  {
    kLeft = 0,  // Left eye
    kRight = 1, // Right eye
    kNone = 2,  // No eye
  };
  using XRViewSpaceType = XREye;

  // Forward declarations
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
   * @class XRSessionRequestInit
   * The `XRSessionRequestInit` class represents the options for requesting a new WebXR session.
   */
  class XRSessionRequestInit
  {
  public:
    /**
     * Creates a default `XRSessionRequestInit` instance.
     *
     * @returns A default `XRSessionRequestInit` instance.
     */
    static XRSessionRequestInit Default()
    {
      XRSessionRequestInit init;
      init.requiredFeatures = {
          xr::TrXRFeature::LOCAL,
          xr::TrXRFeature::VIEWER};
      return init;
    }

  public:
    XRSessionRequestInit() = default;

  public:
    std::vector<xr::TrXRFeature> requiredFeatures; // Required features for the session
    std::vector<xr::TrXRFeature> optionalFeatures; // Optional features for the session
    // TODO: Add more options
  };

  /**
   * @class XRSessionConfiguration
   * The `XRSessionConfiguration` class represents the configuration of a new WebXR session.
   */
  class XRSessionConfiguration
  {
  public:
    /**
     * Constructs a new `XRSessionConfiguration` instance.
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
    int id;                                       // Session ID
    XRSessionMode mode;                           // Session mode
    XRSessionRequestInit requestInit;             // Initial request options
    float recommendedContentSize;                 // Recommended content size
    std::vector<xr::TrXRFeature> enabledFeatures; // Enabled features
  };
} // namespace client_xr
