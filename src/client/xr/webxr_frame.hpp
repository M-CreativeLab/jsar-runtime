#pragma once

#include <memory>
#include <chrono>
#include <common/utility.hpp>
#include <bindings/webxr/common.hpp>
#include "./common.hpp"
#include "./webxr_poses.hpp"

namespace client_xr
{
  class XRFrameContext
  {
  public:
    XRFrameContext(xr::TrXRSessionContextData sessionContext,
                   xr::TrXRDeviceContextData deviceContext,
                   std::shared_ptr<XRSession> session)
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
    /**
     * Create a new `xr::TrXRFrameRequest` object for the specified view index.
     * 
     * @param viewIndex the view index.
     * @returns a new `xr::TrXRFrameRequest` object.
     */
    inline std::shared_ptr<xr::TrXRFrameRequest> createFrameRequestForView(uint32_t viewIndex)
    {
      auto req = std::make_shared<xr::TrXRFrameRequest>();
      req->sessionId = sessionId;
      req->stereoId = stereoId;
      req->stereoTimestamp = stereoTimestamp;
      memcpy(req->localBaseMatrix, localBaseMatrix, sizeof(float) * 16);
      memcpy(req->viewerBaseMatrix, viewerBaseMatrix, sizeof(float) * 16);
      req->views[0] = views[0];
      req->views[1] = views[1];
      req->viewIndex = viewIndex;
      return req;
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
    std::shared_ptr<XRSession> session;
    uint32_t time = 0;
  };

  class XRFrame : public std::enable_shared_from_this<XRFrame>,
                  public JSObjectHolder<bindings::XRFrame>
  {
    friend class XRSession;
    friend class XRPose;
    friend class XRInputSource;

  public:
    /**
     * Create a new `XRFrame` object.
     * 
     * @param frameRequest the frame request data.
     * @param session the `XRSession` object.
     * @returns a new `XRFrame` object.
     */
    static std::shared_ptr<XRFrame> Make(xr::TrXRFrameRequest *frameRequest, std::shared_ptr<XRSession> session)
    {
      return std::make_shared<XRFrame>(frameRequest, session);
    }

  public:
    XRFrame(xr::TrXRFrameRequest *frameRequest, std::shared_ptr<XRSession> session);
    XRFrame(XRFrame &other);

  public:
    inline uint32_t id() { return id_; }
    inline uint32_t stereoId() { return stereoId_; }
    inline bool active() { return active_; }
    inline bool animationFrame() { return animationFrame_; }

  public:
    std::shared_ptr<XRSession> session() { return session_; }
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
     * @throws `std::invalid_argument` if `space` or `baseSpace` is `nullptr`.
     * @throws `std::invalid_argument` if `space` is not target ray or grip space.
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
    /**
     * The `getJointPose()` method of the `XRFrame` interface returns an `XRJointPose` object providing the pose of a hand
     * joint (see `XRHand`) relative to a given base space.
     *
     * @param jointSpace An `XRJointSpace` object specifying the joint for which to obtain the pose.
     * @param baseSpace An `XRSpace` object specifying the space to use as the base or origin for the purposes of computing
     *                 the relative position and orientation.
     * @returns an `XRJointPose` object specifying the position and orientation of the joint relative to the `XRSpace`
     *         indicated by `baseSpace`.
     */
    std::shared_ptr<XRJointPose> getJointPose(std::shared_ptr<XRJointSpace> jointSpace, std::shared_ptr<XRSpace> baseSpace);

  private:
    uint32_t id_;
    uint32_t stereoId_;
    uint32_t sessionId_;
    uint32_t timestamp_;
    bool active_;
    bool animationFrame_;
    std::shared_ptr<XRSession> session_;
    std::shared_ptr<XRDeviceClient> device_;
    xr::TrXRFrameRequest *frameRequestData_;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;

  private:
    static thread_local uint32_t NEXT_FRAME_ID;
  };
}