#pragma once

#include <memory>
#include <chrono>
#include "./common.hpp"
#include "./webxr_poses.hpp"

namespace client_xr
{
  class XRFrame
  {
    friend class XRSession;
    friend class XRInputSource;

  public:
    XRFrame();

  public:
    uint32_t id() const { return id_; }
    uint32_t stereoId() const { return stereoId_; }

  public:
    void startFrame();
    void endFrame();

  public:
    // TODO: implement other methods such as `createAnchor`, `createHitTestSource`, `createHitTestSource` etc.
    /**
     * The `XRFrame` method `getPose()` returns the relative position and orientation — the pose — of one `XRSpace` to that
     * of another space. With this, you can observe the motion of objects relative to each other and to fixed locations
     * throughout the scene.
     *
     * @param space an `XRSpace` specifying the space for which to obtain an `XRPose` describing the item's position and
     *              orientation.
     * @param baseSpace An `XRSpace` to use as the base or origin for the purposes of computing the relative position and
     *                  orientation.
     * @returns an `XRPose` object specifying the position and orientation, relative to the `XRSpace` indicated by `baseSpace`.
     */
    std::shared_ptr<XRPose> getPose(std::shared_ptr<XRSpace> space, std::shared_ptr<XRSpace> baseSpace);
    /**
     * The `getViewerPose()` method, a member of the `XRFrame` interface, returns a `XRViewerPose` object which describes the 
     * viewer's pose (position and orientation) relative to the specified reference space.
     * 
     * @param referenceSpace An `XRReferenceSpace` object specifying the space to use as the reference point or base for the 
     *                       computation of the viewer's current pose.
     * @returns a `XRViewerPose` describing the viewer's position and orientation relative to the specified reference space.
     * @throws `InvalidStateError` if `getViewerPose()` was not called within the context of a callback to a session's 
     *         `XRSession.requestAnimationFrame()`.
     */
    std::shared_ptr<XRViewerPose> getViewerPose(std::shared_ptr<XRReferenceSpace> referenceSpace);

  private:
    uint32_t id_;
    uint32_t stereoId_;
    uint32_t sessionId_;
    uint32_t timestamp;
    bool active_;
    bool animationFrame_;
    std::shared_ptr<XRSession> session_;
    std::shared_ptr<XRDeviceClient> device_;
    xr::TrXRFrameRequest *internal_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;

  private:
    static thread_local uint32_t NEXT_FRAME_ID;
  };
}