#include "./session.hpp"
#include "./device.hpp"
#include "renderer/content_renderer.hpp"

namespace xr
{
  TrXRSession::TrXRSession(uint32_t id, Device *xrDevice, TrContentRenderer *contentRenderer, TrXRSessionInit &init)
      : id(id),
        xrDevice(xrDevice),
        contentRenderer(contentRenderer),
        init(init),
        baseMatrix(1.0f),
        boundingInfo(glm::vec3(-0.5), glm::vec3(0.5), baseMatrix)
  {
  }

  TrXRSession::~TrXRSession()
  {
  }

  bool TrXRSession::belongsTo(TrContentRenderer *contentRenderer)
  {
    return this->contentRenderer == contentRenderer;
  }

  bool TrXRSession::belongsTo(pid_t contentPid)
  {
    return contentRenderer->getContentPid() == contentPid;
  }

  bool TrXRSession::isInFrustum()
  {
    return boundingInfo.isInFrustum(xrDevice->m_ViewerFrustumPlanes);
  }

  bool TrXRSession::isCompletelyInFrustum()
  {
    return boundingInfo.isCompletelyInFrustum(xrDevice->m_ViewerFrustumPlanes);
  }
}
