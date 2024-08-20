#include <assert.h>
#include <algorithm>
#include <glm/glm.hpp>

#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
#include "runtime/media_manager.hpp"
#include "math/matrix.hpp"
#include "idgen.hpp"
#include "./device.hpp"

namespace xr
{
  static uint32_t MIN_FRAME_RATE = 60;
  static uint32_t MAX_FRAME_RATE = 90;
  static TrIdGenerator sessionIdGen(1);

  Device *Device::GetInstance()
  {
    return nullptr;
  }

  Device::Device(TrConstellation *constellation)
      : m_Constellation(constellation), m_FieldOfView(0.0f)
  {
    // Initialize the command chan server
    m_CommandChanServer = std::make_unique<ipc::TrOneShotServer<TrXRCommandMessage>>("xrCommandChan");
  }

  void Device::configure(TrDeviceInit &init)
  {
    m_Enabled = true;
    m_StereoRenderingMode = init.stereoRenderingMode;
    DEBUG(LOG_TAG_XR, "configure XRDevice: enabled=%d", m_Enabled);
  }

  void Device::initialize()
  {
    // Initialize the input sources related fields.
    auto &opts = m_Constellation->getOptions();
    m_InputSourcesZone = std::make_unique<TrXRInputSourcesZone>(opts.getZoneFilename("inputsources"), TrZoneType::Server);

    // Start command client watcher.
    startCommandClientWatcher();
  }

  void Device::shutdown()
  {
    m_CommandClientWatcherRunning = false;
    if (m_CommandClientWatcher != nullptr)
      m_CommandClientWatcher->join();
  }

  bool Device::isSessionSupported(xr::TrXRSessionMode mode)
  {
    return mode == xr::TrXRSessionMode::ImmersiveAR;
  }

  shared_ptr<TrXRSession> Device::requestSession(xr::TrXRSessionMode mode, TrContentRenderer *contentRenderer)
  {
    if (isSessionSupported(mode) == false)
      return nullptr;

    int id;
    int tries = 0;
    while (tries <= 10)
    {
      std::shared_lock<std::shared_mutex> lock(m_MutexForSessions);
      bool idExists = false;
      id = sessionIdGen.get();
      // Search for the session id, if the session id has been added, return false
      for (auto session : m_Sessions)
      {
        if (session->id == id)
        {
          idExists = true;
          break; // When the new id is already in the list, we need to generate a new id.
        }
      }
      if (idExists == false)
        break;
      else
        tries++;
    }

    if (id > 0)
    {
      std::unique_lock<std::shared_mutex> lock(m_MutexForSessions);
      TrXRSessionInit init;
      auto newSession = std::make_shared<TrXRSession>(id, this, contentRenderer, init);
      m_Sessions.push_back(newSession);
      return newSession;
    }
    else
    {
      DEBUG(LOG_TAG_XR, "Failed to generate a new session id, tries=%d", tries);
      return nullptr;
    }
  }

  bool Device::endAndRemoveSession(TrXRSession *sessionToEnd)
  {
    if (sessionToEnd == nullptr)
      return false;

    std::unique_lock<std::shared_mutex> lock(m_MutexForSessions);
    for (auto it = m_Sessions.begin(); it != m_Sessions.end();)
    {
      auto session = *it;
      if (session.get() == sessionToEnd)
      {
        it = m_Sessions.erase(it);
        return true;
      }
      else
      {
        it++;
      }
    }
    return false;
  }

  bool Device::enabled()
  {
    return m_Enabled;
  }

  bool Device::isRenderedAsMultipass()
  {
    return m_StereoRenderingMode == TrStereoRenderingMode::MultiPass;
  }

  TrStereoRenderingMode Device::getStereoRenderingMode()
  {
    return m_StereoRenderingMode;
  }

  StereoRenderingFrame *Device::createStereoRenderingFrame()
  {
    return new StereoRenderingFrame(m_StereoRenderingMode == TrStereoRenderingMode::MultiPass);
  }

  Viewport Device::getViewport(int eyeId)
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    return m_ViewportsByEyeId[eyeId];
  }

  float *Device::getViewerBaseMatrix()
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    return m_ViewerBaseMatrix;
  }

  float *Device::getViewMatrixForEye(int eye)
  {
    if (eye != 0 && eye != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::shared_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    return m_ViewerStereoViewMatrix[eye];
  }

  float *Device::getProjectionMatrixForEye(int eye)
  {
    if (eye != 0 && eye != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::shared_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    return m_ViewerStereoProjectionMatrix[eye];
  }

  glm::mat4 Device::getLocalTransform(int id)
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForSessions);
    return getLocalTransformUnsafe(id);
  }

  glm::mat4 Device::getLocalTransformUnsafe(int id)
  {
    // Check for the session if it exists
    if (m_Sessions.size() == 0)
      return glm::mat4(1.0f);

    for (auto session : m_Sessions)
    {
      if (session->id == id)
        return session->getLocalBaseMatrix();
    }
    return glm::mat4(1.0f);
  }

  int Device::getActiveEyeId()
  {
    return m_ActiveEyeId.load();
  }

  void Device::iterateSessionsByContentPid(pid_t contentPid, std::function<void(TrXRSession *)> callback)
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForSessions);
    for (auto session : m_Sessions)
    {
      if (session->belongsTo(contentPid))
        callback(session.get());
    }
  }

  bool Device::updateFov(float fov)
  {
    m_FieldOfView = fov;
    return true;
  }

  bool Device::updateViewport(int eyeId, float x, float y, float width, float height)
  {
    std::unique_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    m_ViewportsByEyeId[eyeId] = Viewport(width, height, x, y);
    return true;
  }

  bool Device::updateViewerBaseMatrix(float *baseMatrixValues)
  {
    std::unique_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    for (int i = 0; i < 16; i++)
      m_ViewerBaseMatrix[i] = baseMatrixValues[i];

    /**
     * Currently we use the viewer's base matrix to update the gaze data when the eye tracking is not supported
     * yet.
     *
     * TODO: support the eye tracking?
     */
    updateGazeFromBaseMatrix(m_ViewerBaseMatrix);

    /**
     * Update the listener's base matrix for the audio engine.
     */
    m_Constellation->mediaManager->updateListenerBaseMatrix(m_ViewerBaseMatrix);
    return true;
  }

  bool Device::updateViewMatrix(int viewIndex, float *viewMatrixValues)
  {
    std::unique_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    if (viewIndex != 0 && viewIndex != 1)
      return false; // Invalid eye id (0 or 1)

    auto &targetViewMatrix = m_ViewerStereoViewMatrix[viewIndex];
    for (int i = 0; i < 16; i++)
      targetViewMatrix[i] = viewMatrixValues[i];
    m_ActiveEyeId = viewIndex;

    // Update the merged frustum by the view matrix
    if (viewIndex == 1)
    {
      auto frustumMatrixL = glm::make_mat4(m_ViewerStereoProjectionMatrix[0]) * glm::make_mat4(m_ViewerStereoViewMatrix[0]);
      auto frustumMatrixR = glm::make_mat4(m_ViewerStereoProjectionMatrix[1]) * glm::make_mat4(m_ViewerStereoViewMatrix[1]);
      if (m_ViewerFrustumPlanes.empty())
        m_ViewerFrustumPlanes = math3d::TrFrustum::GetStereoscopicPlanes(frustumMatrixL, frustumMatrixR);
      else
        math3d::TrFrustum::GetStereoscopicPlanesToRef(frustumMatrixL, frustumMatrixR, m_ViewerFrustumPlanes);
    }
    return true;
  }

  bool Device::updateProjectionMatrix(int viewIndex, float *projectionMatrixValues)
  {
    std::unique_lock<std::shared_mutex> lock(m_MutexForValueUpdates);
    if (viewIndex != 0 && viewIndex != 1)
      return false; // Invalid eye id (0 or 1)

    auto &targetProjectionMatrix = m_ViewerStereoProjectionMatrix[viewIndex];
    for (int i = 0; i < 16; i++)
      targetProjectionMatrix[i] = projectionMatrixValues[i];
    m_ActiveEyeId = viewIndex;
    return true;
  }

  bool Device::updateLocalTransformBySessionId(int id, float *baseMatrixValues)
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForSessions);
    // Check for the session if it exists
    if (m_Sessions.size() == 0)
      return false;

    for (auto session : m_Sessions)
    {
      if (session->id == id)
      {
        glm::mat4 baseMatrix = glm::make_mat4(baseMatrixValues);
        session->setLocalBaseMatrix(baseMatrix);
        return true;
      }
    }
    return false;
  }

  bool Device::updateLocalTransformByDocumentId(int id, float *baseMatrixValues)
  {
    std::shared_lock<std::shared_mutex> lock(m_MutexForSessions);
    if (m_Sessions.size() == 0)
      return false;

    auto content = m_Constellation->contentManager->getContent(id);
    if (TR_UNLIKELY(content == nullptr))
      return false;
    auto session = content->getActiveXRSession();
    if (session == nullptr)
      return false;

    glm::mat4 baseMatrix = glm::make_mat4(baseMatrixValues);
    session->setLocalBaseMatrix(baseMatrix);
    return true;
  }

  // InputSource

  string Device::getInputSourcesZonePath()
  {
    if (!m_Enabled)
      return "";
    else
      return m_InputSourcesZone->getFilename();
  }

  void Device::syncInputSourcesToZone() { return m_InputSourcesZone->syncData(); }
  void Device::configureMainControllerInputSource(bool enabled, bool usingTouch)
  {
    auto mainController = m_InputSourcesZone->getMainControllerInputSource();
    mainController->enabled = enabled;
    mainController->handness = TrHandness::None;
    mainController->targetRayMode = usingTouch ? TrXRTargetRayMode::Screen : TrXRTargetRayMode::TrackedPointer;
  }
  TrXRInputSource *Device::getGazeInputSource() { return m_InputSourcesZone->getGazeInputSource(); }
  TrXRInputSource *Device::getMainControllerInputSource() { return m_InputSourcesZone->getMainControllerInputSource(); }
  TrXRInputSource *Device::getHandInputSource(int id) { return m_InputSourcesZone->getHandInputSource(id); }
  TrXRInputSource *Device::getHandInputSource(TrHandness handness) { return m_InputSourcesZone->getHandInputSource(handness); }

  int Device::getCommandChanPort() { return m_CommandChanServer->getPort(); }
  void Device::startCommandClientWatcher()
  {
    m_CommandClientWatcherRunning = true;
    m_CommandClientWatcher = std::make_unique<thread>([this]()
                                                      {
      while (m_CommandClientWatcherRunning)
      {
        m_CommandChanServer->tryAccept([this](ipc::TrOneShotClient<TrXRCommandMessage>& newClient){
          auto content = m_Constellation->contentManager->getContent(newClient.getCustomId(), true);
          if (content == nullptr)
            m_CommandChanServer->removeClient(&newClient);
          else
            content->onXRCommandChanConnected(newClient);
        }, m_AcceptTimeout);
      } });
  }

#define TR_XRCOMMAND_METHODS_MAP(XX) \
  XX(IsSessionSupportedRequest)      \
  XX(SessionRequest)                 \
  XX(EndSessionRequest)              \
  XX(SetInputSourceTargetRayHitTestResult)

  void Device::handleCommandMessage(TrXRCommandMessage &message, TrContentRuntime *content)
  {
    TrContentRenderer *contentRenderer = m_Constellation->renderer->findContentRenderer(content);
    if (contentRenderer == nullptr)
      return; // Just ignore the XR command message if the content renderer is not found.

    auto type = message.type;
    switch (type)
    {
#define XX(hander)                                   \
  case xr::TrXRCmdType::hander:                      \
  {                                                  \
    auto req = message.createInstance<xr::hander>(); \
    on##hander(*req, contentRenderer);               \
    delete req;                                      \
    break;                                           \
  }
      TR_XRCOMMAND_METHODS_MAP(XX)
#undef XX
    default:
      DEBUG(LOG_TAG_XR, "Unknown XR command type: %d", (int)type);
      break;
    }
  }

  void Device::onIsSessionSupportedRequest(xr::IsSessionSupportedRequest &request, TrContentRenderer *contentRenderer)
  {
    xr::IsSessionSupportedResponse resp;
    resp.supported = isSessionSupported(request.sessionMode);
    contentRenderer->getContent()->sendXRCommandResponse(resp);
  }

  void Device::onSessionRequest(xr::SessionRequest &request, TrContentRenderer *contentRenderer)
  {
    auto newSession = requestSession(request.sessionMode, contentRenderer);
    xr::SessionResponse resp(newSession->id);
    resp.recommendedContentSize = newSession->recommendedContentSize;
    contentRenderer->getContent()->sendXRCommandResponse(resp);
  }

  void Device::onEndSessionRequest(xr::EndSessionRequest &request, TrContentRenderer *contentRenderer)
  {
    // TODO: implement the end session request
  }

  void Device::onSetInputSourceTargetRayHitTestResult(xr::SetInputSourceTargetRayHitTestResult &request, TrContentRenderer *contentRenderer)
  {
    auto inputSource = m_InputSourcesZone->getInputSourceById(request.inputSourceId);
    if (inputSource != nullptr)
    {
      // Update ray hit
      TrRayHitResult hitResult(request.hit, request.rayEndBaseMatrix);
      inputSource->setTargetRayHitResult(hitResult);
    }
  }
}
