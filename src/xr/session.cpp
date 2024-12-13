#include "./session.hpp"
#include "./device.hpp"
#include "renderer/content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/media_manager.hpp"

namespace xr
{
  TrXRSession::TrXRSession(uint32_t id, Device *xrDevice, TrContentRenderer *contentRenderer,
                           TrXRSessionMode mode, TrXRSessionInit &init)
      : id(id),
        xrDevice(xrDevice),
        constellation(xrDevice->m_Constellation),
        contentRenderer(contentRenderer),
        mode(mode),
        init(init),
        baseMatrix(1.0f),
        boundingInfo(glm::vec3(-recommendedContentSize / 2), glm::vec3(recommendedContentSize / 2), baseMatrix)
  {
    auto content = contentRenderer->getContent();
    assert(content != nullptr);
    content->appendXRSession(this);

    auto constellation = xrDevice->m_Constellation;
    auto &opt = constellation->getOptions();
    contextZone = make_unique<TrXRSessionContextZone>(opt.getZoneFilename(std::to_string(id), "sessions"),
                                                      TrZoneType::Server, id);
  }

  TrXRSession::~TrXRSession()
  {
    auto content = contentRenderer->getContent();
    if (content != nullptr)
      content->removeXRSession(this);
  }

  void TrXRSession::tick()
  {
    // TODO: need to check if this session is active?
    contextZone->syncData();
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

    // Update to the context zone.
    contextZone->setLocalBaseMatrix(glm::value_ptr(baseMatrix));

    // Update the sound sources' base matrix.
    auto content = contentRenderer->getContent();
    constellation->mediaManager->iterateSoundSourcesByContent(content, [this](shared_ptr<TrSoundSource> soundSource)
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
