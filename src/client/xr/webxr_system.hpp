#pragma once

#include <memory>
#include <optional>
#include "./common.hpp"
#include "./webxr_session.hpp"
#include "../dom/dom_event_target.hpp"

namespace client_xr
{
  class XRDeviceClient;
  class XRSystem : public dom::DOMEventTarget,
                   public std::enable_shared_from_this<XRSystem>
  {
  public:
    /**
     * Create a new instance of the XRSystem with a `XRDeviceClient` instance.
     *
     * @param device The `XRDeviceClient` instance to use.
     * @param eventloop The libuv's event loop to use.
     * @returns The new instance of the XRSystem.
     */
    static std::shared_ptr<XRSystem> Make(std::shared_ptr<XRDeviceClient> device, uv_loop_t *eventloop)
    {
      return std::make_shared<XRSystem>(device, eventloop);
    }

  public:
    XRSystem(std::shared_ptr<XRDeviceClient> device, uv_loop_t *eventloop)
        : dom::DOMEventTarget()
        , device_(device)
        , eventloop_(eventloop)
    {
      assert(device_ != nullptr);
      assert(eventloop_ != nullptr);
    }

  public:
    /**
     * @returns the `XRDeviceClient` instance that the `XRSystem` is using.
     */
    std::shared_ptr<XRDeviceClient> device() const
    {
      return device_;
    }
    /**
     * @returns the libuv's event loop that the `XRSystem` is using.
     */
    uv_loop_t *eventloop() const
    {
      return eventloop_;
    }

  public:
    /**
     * The `XRSystem` method `isSessionSupported()` returns `true` if the specified WebXR session mode is supported by the
     * user's WebXR device. Otherwise, the promise resolves with `false`.
     *
     * @param mode specifying the WebXR session mode for which support is to be checked.
     * @returns `true` if the specified session mode is supported, otherwise `false`.
     */
    bool isSessionSupported(XRSessionMode mode);
    /**
     * The `XRSystem` interface's `requestSession()` method returns an XRSession object through which you can manage the
     * requested type of WebXR session.
     *
     * @param mode specifying the WebXR session mode to request.
     * @param init an object to configure the `XRSession`. If none are included, the device will use a default feature
     *             configuration for all options.
     * @returns an `XRSession` object if the device supports the requested mode and features.
     */
    std::shared_ptr<XRSession> requestSession(XRSessionMode mode = XRSessionMode::ImmersiveAR,
                                              std::optional<XRSessionRequestInit> init = std::nullopt);

  private:
    std::shared_ptr<XRDeviceClient> device_;
    uv_loop_t *eventloop_;
  };
}
