#include "./device.hpp"
#include "./common.hpp"

using namespace std;

namespace endor
{
  namespace client_xr
  {
    shared_ptr<XRSystem> XRDeviceClient::createXRSystem(uv_loop_t *eventloop)
    {
      assert(xrSystem_ == nullptr && "XRSystem is already created.");
      assert(eventloop != nullptr && "The event loop must not be null.");

      xrSystem_ = XRSystem::Make(shared_from_this(), eventloop);
      return xrSystem_;
    }

    bool XRDeviceClient::isSessionSupported(XRSessionMode mode)
    {
      if (mode == xr::TrXRSessionMode::Unknown)
        throw invalid_argument("mode should be 'immersive-ar', 'immersive-vr' or 'inline'.");

      xr::IsSessionSupportedRequest request(mode);
      if (!clientContext_->sendXrCommand(request))
        throw runtime_error("failed to send XR command(IsSessionSupportedRequest).");

      auto resp = clientContext_->recvXrCommand<xr::IsSessionSupportedResponse>(
        xr::TrXRCmdType::IsSessionSupportedResponse,
        requestTimeout_);
      if (resp == nullptr)
        throw runtime_error("failed to receive XR command(SessionResponse).");

      bool supported = resp->supported;
      delete resp;
      return supported;
    }

    bool XRDeviceClient::supportsFeature(xr::TrXRFeature feature)
    {
      switch (feature)
      {
      case xr::TrXRFeature::LOCAL:
      case xr::TrXRFeature::VIEWER:
      case xr::TrXRFeature::HAND_TRACKING:
        return true;
      default:
        return false;
      }
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

    XRSessionConfiguration XRDeviceClient::requestSession(XRSessionMode mode,
                                                          vector<xr::TrXRFeature> features,
                                                          optional<XRSessionRequestInit> init)
    {
      xr::SessionRequest request(mode);
      if (!clientContext_->sendXrCommand(request))
        throw runtime_error("failed to send XR command(SessionRequest).");

      auto resp = clientContext_->recvXrCommand<xr::SessionResponse>(xr::TrXRCmdType::SessionResponse, requestTimeout_);
      if (resp == nullptr)
        throw runtime_error("failed to receive XR command(SessionResponse).");

      XRSessionConfiguration sessionConfig(*resp, mode, init.value_or(XRSessionRequestInit::Default()), features);
      delete resp;
      return sessionConfig;
    }

    XRSessionConfiguration XRDeviceClient::requestSession(string modeString,
                                                          vector<xr::TrXRFeature> features,
                                                          optional<XRSessionRequestInit> init)
    {
      xr::TrXRSessionMode mode = xr::MakeSessionMode(modeString);
      if (mode == xr::TrXRSessionMode::Unknown)
        throw invalid_argument("mode should be 'immersive-ar', 'immersive-vr' or 'inline'.");
      return requestSession(mode, features, init);
    }

    bool XRDeviceClient::startFrame(shared_ptr<XRSession> session, xr::TrXRFrameRequest *frameRequest)
    {
      assert(session != nullptr);
      session->markCurrentFrameStarted(frameRequest);

      auto glContext = session->glContext();
      if (TR_UNLIKELY(glContext == nullptr))
      {
        auto msg = "WebGLContext is not connected with the XRSession(" + std::to_string(session->id) + ").";
        throw runtime_error(msg);
      }
      auto req = session->createStartFrameCommand();
      return glContext->sendCommandBufferRequestDirectly(req);
    }

    bool XRDeviceClient::endFrame(shared_ptr<XRSession> session, xr::TrXRFrameRequest *frameRequest)
    {
      assert(session != nullptr);
      session->markCurrentFrameEnded();
      auto glContext = session->glContext();
      if (TR_UNLIKELY(glContext == nullptr))
      {
        auto msg = "WebGLContext is not connected with the XRSession(" + std::to_string(session->id) + ").";
        throw runtime_error(msg);
      }
      auto req = session->createEndFrameCommand(frameRequest);
      return glContext->sendCommandBufferRequestDirectly(req);
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
} // namespace endor
