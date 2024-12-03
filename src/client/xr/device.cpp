#include "./device.hpp"
#include "common/xr/message.hpp"

namespace client_xr
{
  bool XRDeviceClient::isSessionSupported(XRSessionMode mode)
  {
    if (mode == xr::TrXRSessionMode::Unknown)
      throw std::invalid_argument("mode should be 'immersive-ar', 'immersive-vr' or 'inline'.");

    xr::IsSessionSupportedRequest request(mode);
    if (!clientContext->sendXrCommand(request))
      throw std::runtime_error("failed to send XR command(IsSessionSupportedRequest).");

    auto resp = clientContext->recvXrCommand<xr::IsSessionSupportedResponse>(xr::TrXRCmdType::IsSessionSupportedResponse, requestTimeout);
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
}
