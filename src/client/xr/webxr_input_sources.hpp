#pragma once

#include <memory>
#include <optional>

#include "./common.hpp"
#include "./webxr_spaces.hpp"
#include "./webxr_hand.hpp"

namespace client_xr
{
  using InputSourceInternalResetCallback = function<xr::TrXRInputSource *(xr::TrXRFrameRequest *)>;
  using InputSourcesChangedCallback = function<void(vector<XRInputSource *> added, vector<XRInputSource *> removed)>;

  /**
   * The WebXR Device API's `XRInputSource` interface describes a single source of control input which is part of the user's
   * WebXR-compatible virtual or augmented reality system. The device is specific to the platform being used, but provides the
   * direction in which it is being aimed and optionally may generate events if the user triggers performs actions using the
   * device.
   */
  class XRInputSource
  {
  public:
    XRInputSource(std::shared_ptr<XRSession> session, xr::TrXRInputSource *inputSourceData);

  public:
    /**
     * The read-only `XRInputSource` property `gripSpace` returns an `XRSpace` whose native origin tracks the pose used to render
     * virtual objects so they appear to be held in (or part of) the user's hand. For example, if a user were holding a virtual
     * straight rod, the native origin of this `XRSpace` would be located at the approximate center of mass of the user's fist.
     *
     * @returns An `XRSpace` object representing the position and orientation of the input device in virtual space, suitable for
     *          rendering an image of the device into the scene. `gripSpace` is `null` if the input source is not inherently
     *          trackable. For example, only inputs whose `targetRayMode` is `tracked-pointer` provide a `gripSpace`.
     */
    XRSpace gripSpace();
    /**
     * The read-only `hand` property of the `XRInputSource` interface is a `XRHand` object providing access to a hand-tracking
     * device.
     *
     * @returns An `XRHand` object or `std::nullopt` if the `XRSession` has not been requested with the `hand-tracking` feature
     *          descriptor.
     */
    std::optional<XRHand> hand();
    /**
     * The read-only `XRInputSource` property `handedness` indicates which of the user's hands the WebXR input source is associated
     * with, or if it's not associated with a hand at all.
     *
     * @returns a `XRHandedness` enum value indicating the handedness of the input source.
     */
    XRHandedness handedness();
    /**
     * The read-only `XRInputSource` property `targetRayMode` indicates the method by which the target ray for the input source
     * should be generated and how it should be presented to the user.
     *
     * Typically a target ray is drawn from the source of the targeting system along the target ray in the direction in which
     * the user is looking or pointing. The style of the ray is generally up to you, as is the method for indicating the endpoint
     * of the ray. The targeted point or object might be indicated by drawing a shape or highlighting the targeted surface or
     * object.
     *
     * The target ray can be anything from a simple line (ideally fading over distance) to an animated effect, such as the
     * science-fiction "phaser" style shown in the screenshot above.
     *
     * @returns a `XRTargetRayMode` indicating which method to use when generating and presenting the target ray to the user.
     */
    XRTargetRayMode targetRayMode();
    /**
     * The read-only `XRInputSource` property `targetRaySpace` returns an `XRSpace` (typically an `XRReferenceSpace`) representing
     * the position and orientation of the target ray in the virtual space. Its native origin tracks the position of the origin
     * point of the target ray, and its orientation indicates the orientation of the controller device itself. These values,
     * interpreted in the context of the input source's `targetRayMode`, can be used both to fully interpret the device as an input
     * source.
     *
     * To obtain an XRSpace representing the input controller's position and orientation in virtual space, use the gripSpace property.
     *
     * @returns an `XRSpace` object — typically an `XRReferenceSpace` or `XRBoundedReferenceSpace` — which represents the position and
     *          orientation of the input controller's target ray in virtual space.
     */
    XRSpace targetRaySpace();

  private:
    bool dispatchSelectOrSqueezeEvents(XRFrame &frame);

  private:
    std::shared_ptr<XRSession> session_;
    xr::TrXRInputSource *inputSourceData_;
    bool primaryActionPressed_ = false;
    bool squeezeActionPressed_ = false;
  };

  class XRInputSourceArray : public std::vector<std::shared_ptr<XRInputSource>>
  {
  public:
    XRInputSourceArray(std::shared_ptr<XRSession> session);

  public:
    /**
     * @param id The id of the input source.
     * @returns the input source by id.
     */
    std::shared_ptr<XRInputSource> getInputSourceById(int id);

  private:
    /**
     * Update the input sources.
     *
     * @param frame The `XRFrame` object representing the frame to be updated.
     * @param session The `XRSession` object representing the session to be updated.
     * @param onChangedCallback The callback function to be called when the input sources have changed.
     */
    void updateInputSources(XRFrame &frame, XRSession &session, InputSourcesChangedCallback onChangedCallback);

  private:
    std::shared_ptr<XRSession> session_;
    std::shared_ptr<XRDeviceClient> device_;
  };
}
