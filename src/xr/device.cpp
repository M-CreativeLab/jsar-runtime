#include <assert.h>
#include <algorithm>
#include <glm/glm.hpp>

#include "runtime/constellation.hpp"
#include "runtime/content.hpp"
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

  Device::~Device()
  {
    // Clear the sessions
    {
      for (auto session : m_Sessions)
        delete session;
      m_Sessions.clear();
    }
  }

  void Device::initialize(bool enabled, TrDeviceInit &init)
  {
    m_Enabled = enabled;
    m_StereoRenderingMode = init.stereoRenderingMode;

    // Initialize the input sources related fields.
    auto &constellationOptions = m_Constellation->getOptions();
    m_InputSourcesZone = std::make_unique<TrXRInputSourcesZone>(constellationOptions.getZoneFilename("inputsources"), TrZoneType::Server);

    // Start command client watcher.
    startCommandClientWatcher();
    DEBUG(LOG_TAG_XR, "The XR Device has been configured successfully: enabled=%s.", enabled ? "YES" : "NO");
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

  int Device::requestSession(xr::TrXRSessionMode mode, TrContentRenderer *contentRenderer)
  {
    if (isSessionSupported(mode) == false)
      return 0;

    int id;
    int tries = 0;
    while (tries <= 10)
    {
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
      TrXRSessionInit init;
      m_Sessions.push_back(new TrXRSession(id, contentRenderer, init));
      return id;
    }
    else
    {
      DEBUG(LOG_TAG_XR, "Failed to generate a new session id, tries=%d", tries);
      return 0;
    }
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
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewportsByEyeId[eyeId];
  }

  float *Device::getViewerBaseMatrix()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerBaseMatrix;
  }

  float *Device::getViewMatrixForEye(int eye)
  {
    if (eye != 0 && eye != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerStereoViewMatrix[eye];
  }

  float *Device::getProjectionMatrixForEye(int eye)
  {
    if (eye != 0 && eye != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerStereoProjectionMatrix[eye];
  }

  glm::mat4 Device::getLocalTransform(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
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
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ActiveEyeId;
  }

  void Device::iterateSessionsByContentPid(pid_t contentPid, std::function<void(TrXRSession *)> callback)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto session : m_Sessions)
    {
      if (session->belongsTo(contentPid))
        callback(session);
    }
  }

  bool Device::updateFov(float fov)
  {
    m_FieldOfView = fov;
    return true;
  }

  bool Device::updateViewport(int eyeId, float x, float y, float width, float height)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_ViewportsByEyeId[eyeId] = Viewport(width, height, x, y);
    return true;
  }

  bool Device::updateViewerBaseMatrix(float *baseMatrixValues)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int i = 0; i < 16; i++)
      m_ViewerBaseMatrix[i] = baseMatrixValues[i];

    /**
     * If there is no eye tracking, the target ray transform will be the same as the viewer transform.
     *
     * TODO: support the eye tracking?
     */
    if (m_InputSourcesZone != nullptr)
    {
      auto gazeInputSource = m_InputSourcesZone->getGazeInputSource();
      gazeInputSource->setTargetRayBaseMatrix(baseMatrixValues);
    }
    return true;
  }

  bool Device::updateViewMatrix(int viewIndex, float *viewMatrixValues)
  {
    if (viewIndex != 0 && viewIndex != 1)
      return false; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    auto &targetViewMatrix = m_ViewerStereoViewMatrix[viewIndex];

    for (int i = 0; i < 16; i++)
      targetViewMatrix[i] = viewMatrixValues[i];
    m_ActiveEyeId = viewIndex;
    return true;
  }

  bool Device::updateProjectionMatrix(int viewIndex, float *projectionMatrixValues)
  {
    if (viewIndex != 0 && viewIndex != 1)
      return false; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    auto &targetProjectionMatrix = m_ViewerStereoProjectionMatrix[viewIndex];

    for (int i = 0; i < 16; i++)
      targetProjectionMatrix[i] = projectionMatrixValues[i];
    m_ActiveEyeId = viewIndex;
    return true;
  }

  bool Device::updateLocalTransform(int id, float *transform)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    // Check for the session if it exists
    if (m_Sessions.size() == 0)
      return false;

    std::array<float, 16> input = {
        transform[0], transform[1], transform[2], transform[3],
        transform[4], transform[5], transform[6], transform[7],
        transform[8], transform[9], transform[10], transform[11],
        transform[12], transform[13], transform[14], transform[15]};
    // auto rightHanded = math::ConvertMatrixToRightHanded(input);
    for (auto session : m_Sessions)
    {
      if (session->id == id)
      {
        glm::mat4 localBaseMatrix = glm::make_mat4(input.data());
        session->setLocalBaseMatrix(localBaseMatrix);
        return true;
      }
    }
    return false;
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
  TrXRInputSource *Device::getGazeInputSource() { return m_InputSourcesZone->getGazeInputSource(); }
  TrXRInputSource *Device::getHandInputSource(int id) { return m_InputSourcesZone->getHandInputSource(id); }
  TrXRInputSource *Device::getHandInputSource(TrHandness handness) { return m_InputSourcesZone->getHandInputSource(handness); }
  TrXRInputSource *Device::getGamepadInputSource(int id) { return m_InputSourcesZone->getGamepadInputSource(id); }
  TrXRInputSource *Device::getScreenInputSource(int id) { return m_InputSourcesZone->getScreenInputSource(id); }
  void Device::addGamepadInputSource(TrXRInputSource &newInputSource) { return m_InputSourcesZone->addGamepadInputSource(newInputSource); }
  void Device::removeGamepadInputSource(int id) { m_InputSourcesZone->removeGamepadInputSource(id); }
  void Device::addScreenInputSource(TrXRInputSource &newInputSource) { m_InputSourcesZone->addScreenInputSource(newInputSource); }
  void Device::removeScreenInputSource(int id) { m_InputSourcesZone->removeScreenInputSource(id); }

  int Device::getCommandChanPort() { return m_CommandChanServer->getPort(); }
  void Device::startCommandClientWatcher()
  {
    m_CommandClientWatcherRunning = true;
    m_CommandClientWatcher = std::make_unique<thread>([this]()
                                                      {
      while (m_CommandClientWatcherRunning)
      {
        auto newClient = m_CommandChanServer->tryAccept(m_AcceptTimeout);
        if (newClient != nullptr)
        {
           auto content = m_Constellation->getContentManager()->findContent(newClient->getPid());
          if (content == nullptr)
            m_CommandChanServer->removeClient(newClient);
          else
            content->setupWithXRCommandBufferClient(newClient);
        }
      } });
  }

#define TR_XRCOMMAND_METHODS_MAP(XX) \
  XX(IsSessionSupportedRequest)      \
  XX(SessionRequest)

  void Device::handleCommandMessage(TrXRCommandMessage &message, TrContentRuntime *content)
  {
    TrContentRenderer *contentRenderer = m_Constellation->getRenderer()->findContentRenderer(content);
    if (contentRenderer == nullptr)
      return; // Just ignore the XR command message if the content renderer is not found.

    auto type = message.type;
    switch (type)
    {
#define XX(hander)                                   \
  case xr::TrXRCmdType::hander:                      \
  {                                                  \
    auto req = message.createInstance<xr::hander>(); \
    auto res = on##hander(*req, contentRenderer);    \
    content->sendXRCommandResponse(res);             \
    delete req;                                      \
    break;                                           \
  }
      TR_XRCOMMAND_METHODS_MAP(XX)
#undef XX
    default:
      DEBUG(LOG_TAG_XR, "Unknown command type: %d", (int)type);
      break;
    }
  }

  xr::IsSessionSupportedResponse Device::onIsSessionSupportedRequest(xr::IsSessionSupportedRequest &request, TrContentRenderer *contentRenderer)
  {
    xr::IsSessionSupportedResponse resp;
    resp.supported = isSessionSupported(request.sessionMode);
    return resp;
  }

  xr::SessionResponse Device::onSessionRequest(xr::SessionRequest &request, TrContentRenderer *contentRenderer)
  {
    auto sessionId = requestSession(request.sessionMode, contentRenderer);
    xr::SessionResponse resp(sessionId);
    return resp;
  }

}
