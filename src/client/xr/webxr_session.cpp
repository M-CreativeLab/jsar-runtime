#include "./webxr_session.hpp"
#include "../per_process.hpp"

using namespace std;

namespace client_xr
{
  XRSession::XRSession(XRSessionConfiguration config, std::shared_ptr<XRDeviceClient> device)
      : id(config.id),
        device_(device),
        mode(config.mode),
        requestInit(config.requestInit)
  {
    auto clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);

    // Create the session context zone client.
    string zonePath = clientContext->xrDeviceInit.sessionContextZoneDirectory + "/" + std::to_string(id);
    sessionContextZoneClient_ = xr::TrXRSessionContextZone::Make(zonePath, TrZoneType::Client);

    // Create view spaces
    if (immersive())
    {
      addViewSpace(XRViewSpaceType::kLeft);
      addViewSpace(XRViewSpaceType::kRight);
    }
    else
    {
      addViewSpace(XRViewSpaceType::kNone);
    }

    activeRenderState_ = std::make_unique<XRRenderState>();
    pendingRenderState_ = nullptr;

    viewerSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kViewer);
    localSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kLocal);
    unboundedSpace_ = std::make_shared<XRReferenceSpace>(XRReferenceSpaceType::kUnbounded);
  }

  XRSession::~XRSession()
  {
  }

  void XRSession::addViewSpace(XRViewSpaceType type)
  {
    viewSpaces_.push_back(XRViewSpace::Make(type));
  }
}
