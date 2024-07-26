#include "./session.hpp"
#include "./device.hpp"
#include "renderer/content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/media_manager.hpp"

namespace xr
{
  TrXRSession::TrXRSession(uint32_t id, Device *xrDevice, TrContentRenderer *contentRenderer, TrXRSessionInit &init)
      : id(id),
        xrDevice(xrDevice),
        constellation(xrDevice->m_Constellation),
        contentRenderer(contentRenderer),
        init(init),
        baseMatrix(1.0f),
        boundingInfo(glm::vec3(-0.5), glm::vec3(0.5), baseMatrix)
  {
    auto content = contentRenderer->getContent();
    assert(content != nullptr);
    content->appendXRSession(this);
  }

  TrXRSession::~TrXRSession()
  {
    auto content = contentRenderer->getContent();
    assert(content != nullptr);
    content->removeXRSession(this);
  }

  bool TrXRSession::belongsTo(TrContentRenderer *contentRenderer)
  {
    return this->contentRenderer == contentRenderer;
  }

  bool TrXRSession::belongsTo(pid_t contentPid)
  {
    return contentRenderer->getContentPid() == contentPid;
  }

  bool TrXRSession::isActive()
  {
    auto content = contentRenderer->getContent();
    assert(content != nullptr);
    return content->getActiveXRSession() == this;
  }

  glm::mat4 TrXRSession::getLocalBaseMatrix()
  {
    return baseMatrix;
  }

  void TrXRSession::setLocalBaseMatrix(glm::mat4 matrix)
  {
    baseMatrix = matrix;
    boundingInfo.update(baseMatrix);

    // Update the sound sources' base matrix.
    auto content = contentRenderer->getContent();
    constellation->getMediaManager()->iterateSoundSourcesByContent(content, [this](shared_ptr<TrSoundSource> soundSource)
                                                                   { soundSource->setBaseMatrix(baseMatrix); });
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
