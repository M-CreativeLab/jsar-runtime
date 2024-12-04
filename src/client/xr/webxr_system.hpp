#pragma once

#include <memory>
#include <optional>
#include "./common.hpp"
#include "./webxr_session.hpp"

namespace client_xr
{
  class XRDeviceClient;
  class XRSystem
  {
  public:
    /**
     * Create a new instance of the XRSystem with a `XRDeviceClient` instance.
     *
     * @param xrDevice The XRDeviceClient instance to use.
     * @returns The new instance of the XRSystem.
     */
    static std::shared_ptr<XRSystem> Make(std::shared_ptr<XRDeviceClient> xrDevice)
    {
      return std::make_shared<XRSystem>(xrDevice);
    }

  public:
    XRSystem(std::shared_ptr<XRDeviceClient> xrDevice)
        : xrDevice_(xrDevice)
    {
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
    std::shared_ptr<XRSession> requestSession(XRSessionMode mode, std::optional<XRSessionRequestInit> init = std::nullopt);

  private:
    std::shared_ptr<XRDeviceClient> xrDevice_;
  };
}
