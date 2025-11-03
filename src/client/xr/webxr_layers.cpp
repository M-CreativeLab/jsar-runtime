#include "./webxr_layers.hpp"
#include "./webxr_session.hpp"
#include "./device.hpp"

namespace endor
{
  namespace client_xr
  {
    using namespace std;

    XRLayer::XRLayer(shared_ptr<XRSession> session)
        : session_(session)
    {
    }

    XRWebGLLayer::XRWebGLLayer(shared_ptr<XRSession> session, shared_ptr<client_graphics::WebGLContext> glContext)
        : XRLayer(session)
        , xr::WebGLLayer()
        , glContext_(glContext)
    {
      auto device = session->device();
      multiviewRequired = device->getDeviceInit().multiviewRequired();

      auto deviceContext = device->contextZone();
      if (deviceContext != nullptr)
      {
        const auto &framebufferConfig = deviceContext->getFramebufferConfig();
        framebufferWidth = framebufferConfig.width;
        framebufferHeight = framebufferConfig.height;
      }
    }

    XRWebGLLayer::XRWebGLLayer(xr::WebGLLayer &layerData)
        : XRLayer(nullptr)
        , xr::WebGLLayer(layerData)
        , glContext_(nullptr)
    {
    }

    XRViewport &XRWebGLLayer::getViewport(std::shared_ptr<XRView> view)
    {
      return view->viewport();
    }

    XRCompositionLayer::XRCompositionLayer(shared_ptr<XRSession> session)
        : XRLayer(session)
    {
    }
  }
} // namespace endor
