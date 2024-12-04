#pragma once

#include <memory>
#include <vector>
#include "../per_process.hpp"
#include "./common.hpp"
#include "./webxr_system.hpp"

namespace client_xr
{
  /**
   * The callback function for the XR frame request.
   */
  typedef std::function<void(Napi::Env env, xr::TrXRFrameRequest *, void *context)> XRFrameCallback;

  /**
   * The `XRFrameCallback` with context.
   *
   * TODO(yorkie): Use `std::function` instead of this class.
   */
  class ContextifiedXRFrameCallback
  {
  public:
    /**
     * Create a new instance of the `ContextifiedXRFrameCallback`.
     */
    ContextifiedXRFrameCallback(XRFrameCallback callback, void *context) : callback(callback), context(context) {}

  public:
    /**
     * The callback function.
     */
    XRFrameCallback callback;
    /**
     * The context to pass to the callback function.
     */
    void *context = nullptr;
  };

  /**
   * The XRDevice client class.
   *
   * In the client-side, using this class `XRDeviceClient` to interact with the WebXR device.
   */
  class XRDeviceClient : public std::enable_shared_from_this<XRDeviceClient>
  {
  public:
    /**
     * Create a new instance of the XRDeviceClient.
     *
     * @param clientContext The client context to use.
     * @returns The new instance of the XRDeviceClient.
     */
    static std::shared_ptr<XRDeviceClient> Make(TrClientContextPerProcess *clientContext)
    {
      return std::make_shared<XRDeviceClient>(clientContext);
    }

  public:
    explicit XRDeviceClient(TrClientContextPerProcess *clientContext)
        : clientContext_(clientContext)
    {
    }

  public:
    /**
     * @returns the WebXR `XRSystem` instance.
     */
    inline std::shared_ptr<XRSystem> getXRSystem()
    {
      if (xrSystem_ == nullptr)
        xrSystem_ = XRSystem::Make(shared_from_this());
      return xrSystem_;
    }
    /**
     * It returns `true` if the specified WebXR session mode is supported by the user's WebXR device.
     *
     * @param mode The session mode to check.
     * @returns `true` if the session mode is supported.
     */
    bool isSessionSupported(XRSessionMode mode);
    /**
     * It returns `true` if the specified reference space type is supported by the user's WebXR device.
     *
     * @param referenceSpaceType The reference space type to check.
     * @returns `true` if the reference space type is supported.
     */
    bool supportsReferenceSpaceType(XRReferenceSpaceType referenceSpaceType);
    /**
     * It requests a new WebXR session.
     *
     * @param mode The session mode to request.
     * @param init The options to request the session.
     * @returns The session configuration to create a new `XRSession` instance.
     */
    XRSessionConfiguration requestSession(XRSessionMode mode, std::optional<XRSessionRequestInit> init = std::nullopt);
    /**
     * It requests a new WebXR session with the unresolved mode string.
     *
     * @param modeString The unresolved mode string to request, such as `immersive-ar`, `immersive-vr`, or `inline`
     * @param init The options to request the session.
     * @returns The session configuration to create a new `XRSession` instance.
     */
    XRSessionConfiguration requestSession(std::string modeString, std::optional<XRSessionRequestInit> init = std::nullopt);
    /**
     * Request a new WebXR frame.
     *
     * @param callback The callback function to call when the frame is ready.
     * @param context The context to pass to the callback function.
     */
    void requestFrame(XRFrameCallback callback, void *context);
    /**
     * Starts a new WebXR frame.
     *
     * @param frameRequest The frame request to start.
     * @returns `true` if the frame is started successfully.
     */
    bool startFrame(xr::TrXRFrameRequest *frameRequest);
    /**
     * Ends the current WebXR frame.
     *
     * @param frameRequest The frame request to end.
     * @returns `true` if the frame is ended successfully.
     */
    bool endFrame(xr::TrXRFrameRequest *frameRequest);
    /**
     * Get the viewport for the specified view index.
     *
     * @param viewIndex The view index, 0 or 1.
     * @returns The viewport.
     */
    TrViewport getViewport(uint32_t viewIndex);
    /**
     * @returns the WebXR device initialization information.
     */
    xr::TrDeviceInit &getDeviceInit();

  private:
    TrClientContextPerProcess *clientContext_ = nullptr;
    std::vector<ContextifiedXRFrameCallback> contextifiedFrameCallbacks_;
    std::shared_ptr<XRSystem> xrSystem_;
    int requestTimeout_ = 1000;
  };
}
