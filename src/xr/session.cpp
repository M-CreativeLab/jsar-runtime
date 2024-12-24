#include "./session.hpp"
#include "./device.hpp"
#include "renderer/content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/media_manager.hpp"

namespace xr
{
  using namespace std;

  TrXRSession::TrXRSession(uint32_t id, Device *xrDevice, shared_ptr<TrContentRenderer> contentRenderer,
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
    static TrIdGenerator stereoIdGenerator(0x567);
    switch (xrDevice->getStereoRenderingMode())
    {
    case xr::TrStereoRenderingMode::MultiPass:
    {
      if (xrDevice->getActiveEyeId() == 0) // Update the `nextStereoId` only when rendering the left eye.
        nextStereoId = stereoIdGenerator.get();
      break;
    }
    case xr::TrStereoRenderingMode::SinglePass:
    case xr::TrStereoRenderingMode::SinglePassInstanced:
    case xr::TrStereoRenderingMode::SinglePassMultiview:
    {
      nextStereoId = stereoIdGenerator.get(); // Update the `nextStereoId` for each frame.
      break;
    }
    default:
      break;
    }

    // Update the stereo id and zone states.
    if (nextStereoId >= stereoIdGenerator.min())
    {
      setStereoId(nextStereoId);
      updateStatesInZone();
    }

    // TODO: need to check if this session is active?
    contextZone->syncData();
  }

  bool TrXRSession::belongsTo(shared_ptr<TrContentRenderer> contentRenderer)
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
