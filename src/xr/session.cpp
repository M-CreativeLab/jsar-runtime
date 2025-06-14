#include "./session.hpp"
#include "./device.hpp"
#include "renderer/content_renderer.hpp"
#include "runtime/content.hpp"
#include "runtime/media_manager.hpp"

namespace xr
{
  using namespace std;

  TrXRSession::TrXRSession(uint32_t id, Device *device, shared_ptr<TrContentRuntime> content, TrXRSessionMode mode, TrXRSessionInit &init)
      : id(id)
      , device(device)
      , constellation(device->m_Constellation)
      , content(weak_ptr<TrContentRuntime>(content))
      , mode(mode)
      , init(init)
      , baseMatrix(1.0f)
      , boundingInfo(glm::vec3(-recommendedContentSize / 2), glm::vec3(recommendedContentSize / 2), baseMatrix)
  {
    assert(device != nullptr);
    assert(content != nullptr);
    content->appendXRSession(this);

    auto &opt = constellation->getOptions();
    contextZone = make_unique<TrXRSessionContextZone>(opt.getZoneFilename(std::to_string(id), "sessions"),
                                                      TrZoneType::Server,
                                                      id);
  }

  TrXRSession::~TrXRSession()
  {
    auto contentRef = content.lock();
    if (contentRef != nullptr)
      contentRef->removeXRSession(this);
  }

  void TrXRSession::tick()
  {
    static TrIdGenerator stereoIdGenerator(0x567);
    switch (device->getStereoRenderingMode())
    {
    case xr::TrStereoRenderingMode::MultiPass:
    {
      if (device->getActiveEyeId() == 0) // Update the `nextStereoId` only when rendering the left eye.
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

  bool TrXRSession::belongsTo(pid_t contentPid)
  {
    auto contentRef = content.lock();
    return contentRef == nullptr
             ? false
             : contentRef->pid == contentPid;
  }

  bool TrXRSession::isActive()
  {
    auto contentRef = content.lock();
    return contentRef == nullptr
             ? false
             : contentRef->getActiveXRSession() == this;
  }

  glm::mat4 TrXRSession::getLocalBaseMatrix()
  {
    return baseMatrix;
  }

  void TrXRSession::setLocalBaseMatrix(glm::mat4 matrix)
  {
    auto contentRef = content.lock();
    assert(contentRef != nullptr);

    baseMatrix = matrix;
    boundingInfo.update(baseMatrix);

    // Update to the context zone.
    contextZone->setLocalBaseMatrix(glm::value_ptr(baseMatrix));

    // Update the sound sources' base matrix.
    constellation->mediaManager->iterateSoundSourcesByContent(contentRef, [this](shared_ptr<TrSoundSource> soundSource)
                                                              { soundSource->setBaseMatrix(baseMatrix); });
  }

  bool TrXRSession::isInFrustum()
  {
    return boundingInfo.isInFrustum(device->m_ViewerFrustumPlanes);
  }

  bool TrXRSession::isCompletelyInFrustum()
  {
    return boundingInfo.isCompletelyInFrustum(device->m_ViewerFrustumPlanes);
  }
}
