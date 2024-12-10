#include "./webxr_layers.hpp"
#include "./webxr_session.hpp"
#include "./device.hpp"

namespace client_xr
{
  using namespace std;

  XRLayer::XRLayer(shared_ptr<XRSession> session)
      : session_(session)
  {
  }

  XRWebGLLayer::XRWebGLLayer(shared_ptr<XRSession> session, shared_ptr<client_graphics::WebGLContext> glContext)
      : XRLayer(session),
        xr::WebGLLayer(),
        glContext_(glContext)
  {
    multiviewRequired = session->device()->getDeviceInit().multiviewRequired();
  }

  XRWebGLLayer::XRWebGLLayer(xr::WebGLLayer &layerData)
      : XRLayer(nullptr),
        xr::WebGLLayer(layerData),
        glContext_(nullptr)
  {
  }

  XRCompositionLayer::XRCompositionLayer(shared_ptr<XRSession> session)
      : XRLayer(session)
  {
  }
}
