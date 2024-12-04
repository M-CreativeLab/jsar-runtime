#pragma once

#include <memory>
#include <chrono>
#include "./common.hpp"
#include "./webxr_frame.hpp"
#include "./webxr_hand.hpp"
#include "./webxr_input_sources.hpp"
#include "./webxr_poses.hpp"
#include "./webxr_rigid_transform.hpp"
#include "./webxr_layers.hpp"
#include "./webxr_spaces.hpp"
#include "./webxr_view.hpp"
#include "./webxr_viewport.hpp"

namespace client_xr
{
  class XRSession
  {
  public:
    /**
     * Create a new `XRSession` object.
     * 
     * @param config the `XRSessionConfiguration` object, which is returned by the `XRDeviceClient` object.
     * @param device the `XRDeviceClient` object.
     * @returns a new `XRSession` object.
     */
    static std::shared_ptr<XRSession> Make(XRSessionConfiguration config, std::shared_ptr<XRDeviceClient> device)
    {
      return std::make_shared<XRSession>(config, device);
    }

  public:
    XRSession(XRSessionConfiguration config, std::shared_ptr<XRDeviceClient> device)
        : id(config.id),
          device_(device)
    {
    }

  public:
    /**
     * @returns the `XRDeviceClient` object.
     */
    inline std::shared_ptr<XRDeviceClient> device() const { return device_; }
    /**
     * @returns the blend mode to mixing the application content with the host environment.
     */
    XREnvironmentBlendMode environmentBlendMode();
    /**
     * @returns the `XRRenderState` object that represents the current render state of the session.
     */
    XRRenderState *renderState();
    /**
     * It ends the session, and no more frames will be rendered.
     */
    void end();

  private:
    /**
     * Start the session.
     */
    void start();
    /**
     * Stop the session.
     */
    void stop();
    /**
     * Update the session.
     */
    void update();
    /**
     * It calculates the FPS(frames per second) of the session.
     *
     * @returns `true` if the FPS was calculated successfully, `false` otherwise.
     */
    bool calcFps();

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
     * If the session is immersive.
     */
    bool immersive;
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
    std::vector<std::string> enabledFeatures;

  private:
    std::shared_ptr<XRDeviceClient> device_;
    /**
     * The session context zone client.
     */
    std::unique_ptr<xr::TrXRSessionContextZone> sessionContextZoneClient_;
    XREnvironmentBlendMode environmentBlendMode_;
    XRRenderState *ativeRenderState_ = nullptr;
    XRRenderState *pendingRenderState_ = nullptr;
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
  };
}
