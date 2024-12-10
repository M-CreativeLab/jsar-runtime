#pragma once

#include <memory>
#include <chrono>
#include <idgen.hpp>
#include "./common.hpp"
#include "./webxr_frame.hpp"
#include "./webxr_hand.hpp"
#include "./webxr_input_sources.hpp"
#include "./webxr_poses.hpp"
#include "./webxr_renderstate.hpp"
#include "./webxr_rigid_transform.hpp"
#include "./webxr_layers.hpp"
#include "./webxr_spaces.hpp"
#include "./webxr_view.hpp"
#include "./webxr_viewport.hpp"

namespace client_xr
{
  enum class XRSessionUpdateState
  {
    kSuccess = 0,
    kSessionEnded,        // Skip the frame if the session is ended.
    kInvalidSessionId,    // Skip the frame if the session id is invalid.
    kStereoIdMismatch,    // Skip the frame if the stereo id is mismatched.
    kPendingStereoFrames, // Skip the frame if there are more than 2 pending frames.
    kSessionNotInFrustum, // Skip the frame if the session is not in the frustum.
  };

  using XRFrameCallback = std::function<void(uint32_t, XRFrame &)>;
  class XRFrameCallbackWrapper
  {
  public:
    XRFrameCallbackWrapper(XRFrameCallback callback)
        : callback(callback), cancelled(false)
    {
      static TrIdGenerator callbackIdGen(1);
      handle = callbackIdGen.get();
    }
    ~XRFrameCallbackWrapper() {}

  public:
    void cancel() { cancelled = true; }
    void operator()(uint32_t time, XRFrame &frame)
    {
      callback(time, frame);
    }

  public:
    XRFrameCallback callback;
    uint32_t handle;
    bool cancelled;
  };

  class XRSession : public dom::DOMEventTarget,
                    public std::enable_shared_from_this<XRSession>
  {
    friend class XRSystem;
    friend class XRFrame;

  public:
    /**
     * Create a new `XRSession` object.
     *
     * @param config the `XRSessionConfiguration` object, which is returned by the `XRDeviceClient` object.
     * @param xrSystem the `XRSystem` object.
     * @returns a new `XRSession` object.
     */
    static std::shared_ptr<XRSession> Make(XRSessionConfiguration config, std::shared_ptr<XRSystem> xrSystem)
    {
      return std::make_shared<XRSession>(config, xrSystem);
    }

  public:
    XRSession(XRSessionConfiguration config, std::shared_ptr<XRSystem> xrSystem);
    virtual ~XRSession() = default;

  public:
    /**
     * The lifecycle "Primary Action Start" event handler.
     */
    virtual void onPrimaryActionStart(XRInputSource &inputSource, XRFrame &frame) {}
    /**
     * The lifecycle "Primary Action End" event handler.
     */
    virtual void onPrimaryActionEnd(XRInputSource &inputSource, XRFrame &frame) {}
    /**
     * The lifecycle "Squeeze Action Start" event handler.
     */
    virtual void onSqueezeActionStart(XRInputSource &inputSource, XRFrame &frame) {}
    /**
     * The lifecycle "Squeeze Action End" event handler.
     */
    virtual void onSqueezeActionEnd(XRInputSource &inputSource, XRFrame &frame) {}

  public:
    /**
     * @returns the `XRDeviceClient` object.
     */
    inline std::shared_ptr<XRDeviceClient> device() const { return device_; }
    /**
     * @returns `true` if the session is immersive, `false` otherwise.
     */
    inline bool immersive() { return xr::IsImmersive(mode); }
    /**
     * @returns the blend mode to mixing the application content with the host environment.
     */
    inline XREnvironmentBlendMode environmentBlendMode() const { return environmentBlendMode_; }
    /**
     * @returns the `XRRenderState` object that represents the current render state of the session.
     */
    inline XRRenderState renderState() { return XRRenderState(*activeRenderState_); }
    /**
     * @returns the frame time of the session.
     */
    inline std::chrono::steady_clock::time_point frameTime() { return frameTimepoint_; }
    /**
     * @returns the viewer reference space of the session.
     */
    inline std::shared_ptr<XRReferenceSpace> viewerSpace() { return viewerSpace_; }
    /**
     * @returns the local reference space of the session.
     */
    inline std::shared_ptr<XRReferenceSpace> localSpace() { return localSpace_; }

  public:
    /**
     * Update the frame time of the session.
     *
     * @param updateStereoFrame indicates whether the stereo frame time should be updated.
     */
    void updateFrameTime(bool updateStereoFrame);
    /**
     * The `updateRenderState()` method of the `XRSession` interface of the WebXR API schedules changes to be applied to the
     * active render state (`XRRenderState`) prior to rendering of the next frame.
     *
     * @param newState The `XRRenderState` object representing the new render state to be applied.
     */
    void updateRenderState(XRRenderState newState);
    void updateTargetFrameRate(float targetFrameRate);
    void updateCollisionBox(glm::vec3 min, glm::vec3 max);
    void updateInputSourcesIfChanged(XRFrame &frame);
    /**
     * The `requestReferenceSpace()` method of the `XRSession` interface returns a promise that resolves with an instance of
     * either `XRReferenceSpace` or `XRBoundedReferenceSpace` as appropriate given the type of reference space requested.
     *
     * @param type The type of reference space to be requested.
     * @returns an `XRReferenceSpace` object representing the requested reference space.
     */
    std::shared_ptr<XRReferenceSpace> requestReferenceSpace(XRReferenceSpaceType type);
    /**
     * The `requestReferenceSpace()` method of the `XRSession` interface returns a promise that resolves with an instance of
     * either `XRReferenceSpace` or `XRBoundedReferenceSpace` as appropriate given the type of reference space requested.
     *
     * @param typeString A string specifying the type of reference space for which an instance is to be returned. The string
     *                   must be one of the values: "local", "local-floor", "bounded-floor", "unbounded".
     * @returns an `XRReferenceSpace` object representing the requested reference space.
     */
    std::shared_ptr<XRReferenceSpace> requestReferenceSpace(std::string typeString);
    uint32_t requestAnimationFrame(XRFrameCallback callback);
    void cancelAnimationFrame(uint32_t handle);
    /**
     * It ends the session, and no more frames will be rendered.
     */
    void end();

  private:
    /**
     * @returns `true` if the specified feature is enabled, `false` otherwise.
     */
    bool isFeatureEnabled(xr::TrXRFeature feature);
    /**
     * Initialize the session.
     */
    void initialize();
    /**
     * Start the session.
     */
    void start();
    /**
     * Stop the session.
     */
    void stop();
    /**
     * This function will be called in ticks.
     */
    void tick();
    /**
     * Update the session.
     */
    XRSessionUpdateState update();
    /**
     * This function will be called in each WebXR frame.
     */
    void dispatchXRFrame(xr::TrXRFrameRequest &frameRequest);
    /**
     * It calculates the FPS(frames per second) of the session.
     *
     * @returns `true` if the FPS was calculated successfully, `false` otherwise.
     */
    bool calcFps();
    /**
     * It adds a view space to the session with the specified type.
     *
     * @param type the view space type.
     */
    void addViewSpace(XRViewSpaceType type);

  public:
    /**
     * The session id.
     */
    int id;
    /**
     * The session mode.
     */
    XRSessionMode mode;
    /**
     * The initial options when requesting this session.
     */
    XRSessionRequestInit requestInit;
    /**
     * If the session is started.
     */
    bool started;
    /**
     * If the session is ended.
     */
    bool ended;
    /**
     * If the session is suspended.
     */
    bool suspended;
    /**
     * Enabled features.
     */
    std::vector<xr::TrXRFeature> enabledFeatures;
    std::optional<XRInputSourceArray> inputSources;

  private:
    std::shared_ptr<XRDeviceClient> device_;
    /**
     * The session context zone client.
     */
    std::unique_ptr<xr::TrXRSessionContextZone> sessionContextZoneClient_;
    XREnvironmentBlendMode environmentBlendMode_;
    std::unique_ptr<XRRenderState> activeRenderState_ = nullptr;
    std::unique_ptr<XRRenderState> pendingRenderState_ = nullptr;
    std::vector<std::shared_ptr<XRFrameCallbackWrapper>> pendingFrameCallbacks_;
    std::vector<std::shared_ptr<XRFrameCallbackWrapper>> currentFrameCallbacks_;
    std::vector<std::shared_ptr<XRViewSpace>> viewSpaces_;
    std::shared_ptr<XRReferenceSpace> viewerSpace_;
    std::shared_ptr<XRReferenceSpace> localSpace_;
    std::shared_ptr<XRReferenceSpace> unboundedSpace_;
    uint32_t fps_ = 0;
    int framesCount_ = 0;
    /**
     * Every frame timepoint, updated at the start of each frame.
     */
    std::chrono::steady_clock::time_point frameTimepoint_;
    /**
     * The last stereo frame timepoint, updated at the start of each frame from the `frameTimepoint`.
     */
    std::chrono::steady_clock::time_point lastStereoFrameTimepoint_;
    /**
     * The last recorded frame timepoint, updated by manual at calculating FPS.
     */
    std::chrono::steady_clock::time_point lastRecordedFrameTimepoint_ = chrono::steady_clock::now();
    uv_loop_t *eventloop_;
    uv_timer_t tickHandle_;
  };
}
