#include "./device.hpp"
#include "./common.hpp"

namespace client_xr
{
  bool XRDeviceClient::isSessionSupported(XRSessionMode mode)
  {
    if (mode == xr::TrXRSessionMode::Unknown)
      throw std::invalid_argument("mode should be 'immersive-ar', 'immersive-vr' or 'inline'.");

    xr::IsSessionSupportedRequest request(mode);
    if (!clientContext_->sendXrCommand(request))
      throw std::runtime_error("failed to send XR command(IsSessionSupportedRequest).");

    auto resp = clientContext_->recvXrCommand<xr::IsSessionSupportedResponse>(
        xr::TrXRCmdType::IsSessionSupportedResponse,
        requestTimeout_);
    if (resp == nullptr)
      throw std::runtime_error("failed to receive XR command(SessionResponse).");

    bool supported = resp->supported;
    delete resp;
    return supported;
  }

  bool XRDeviceClient::supportsReferenceSpaceType(XRReferenceSpaceType referenceSpaceType)
  {
    if (
        referenceSpaceType == XRReferenceSpaceType::kLocal ||
        referenceSpaceType == XRReferenceSpaceType::kViewer ||
        referenceSpaceType == XRReferenceSpaceType::kUnbounded)
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  XRSessionConfiguration XRDeviceClient::requestSession(XRSessionMode mode, std::optional<XRSessionRequestInit> init)
  {
    xr::SessionRequest request(mode);
    if (!clientContext_->sendXrCommand(request))
      throw std::runtime_error("failed to send XR command(SessionRequest).");

    auto resp = clientContext_->recvXrCommand<xr::SessionResponse>(xr::TrXRCmdType::SessionResponse, requestTimeout_);
    if (resp == nullptr)
      throw std::runtime_error("failed to receive XR command(SessionResponse).");

    XRSessionConfiguration sessionConfig(*resp, mode, init.value_or(XRSessionRequestInit::Default()));
    delete resp;
    return sessionConfig;
  }

  XRSessionConfiguration XRDeviceClient::requestSession(std::string modeString, std::optional<XRSessionRequestInit> init)
  {
    xr::TrXRSessionMode mode = xr::MakeSessionMode(modeString);
    if (mode == xr::TrXRSessionMode::Unknown)
      throw std::invalid_argument("mode should be 'immersive-ar', 'immersive-vr' or 'inline'.");

    return requestSession(mode, init);
  }

  void XRDeviceClient::requestFrame(XRFrameCallback callback, void *context)
  {
    contextifiedFrameCallbacks_.push_back(ContextifiedXRFrameCallback(callback, context));
  }

  bool XRDeviceClient::startFrame(xr::TrXRFrameRequest *frameRequest)
  {
    return clientContext_->startXrFrame(frameRequest);
  }

  bool XRDeviceClient::endFrame(xr::TrXRFrameRequest *frameRequest)
  {
    return clientContext_->finishXrFrame(frameRequest);
  }

  TrViewport XRDeviceClient::getViewport(uint32_t viewIndex)
  {
    int width = clientContext_->getFramebufferWidth();
    int height = clientContext_->getFramebufferHeight();

    if (clientContext_->isFramebufferDoubleWide())
    {
      if (getDeviceInit().stereoRenderingMode != xr::TrStereoRenderingMode::MultiPass)
        width /= 2;
      if (viewIndex == 0)
        return TrViewport(width, height, 0, 0);
      else
        return TrViewport(width, height, width, 0);
    }
    else
    {
      /**
       * Non-double wide framebuffer, the viewports are the same.
       */
      return TrViewport(width, height, 0, 0);
    }
  }

  xr::TrDeviceInit &XRDeviceClient::getDeviceInit()
  {
    return clientContext_->xrDeviceInit;
  }
}
