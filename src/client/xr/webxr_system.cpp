#include "./device.hpp"
#include "./webxr_system.hpp"

using namespace std;

namespace client_xr
{
  bool XRSystem::isSessionSupported(XRSessionMode mode)
  {
    assert(xrDevice_ != nullptr);
    return xrDevice_->isSessionSupported(mode);
  }

  shared_ptr<XRSession> XRSystem::requestSession(XRSessionMode mode, std::optional<XRSessionRequestInit> init)
  {
    return XRSession::Make();
  }
}
