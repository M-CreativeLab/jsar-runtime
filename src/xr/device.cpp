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
      : m_Constellation(constellation), m_FieldOfView(0.0f), m_Time(0.0f)
  {
    m_BackupStereoRenderingFrame = new StereoRenderingFrame(true);

    // Initialize the input sources
    {
      m_GazeInputSource = new InputSource();
      m_GazeInputSource->handness = Handness::None;
      m_GazeInputSource->targetRayMode = TargetRayMode::Gaze;

      auto leftHandInputSource = new InputSource();
      leftHandInputSource->handness = Handness::Left;
      leftHandInputSource->targetRayMode = TargetRayMode::TrackedPointer;
      m_HandInputSources.push_back(leftHandInputSource);

      auto rightHandInputSource = new InputSource();
      rightHandInputSource->handness = Handness::Right;
      rightHandInputSource->targetRayMode = TargetRayMode::TrackedPointer;
      m_HandInputSources.push_back(rightHandInputSource);
    }

    // Initialize the command chan server
    m_CommandChanServer = new ipc::TrOneShotServer<TrXRCommandMessage>("xrCommandChan");
  }

  Device::~Device()
  {
    m_FieldOfView = 0.0f;
    m_Time = 0.0f;
    delete m_BackupStereoRenderingFrame;
    m_BackupStereoRenderingFrame = nullptr;

    // Clear the sessions
    {
      for (auto session : m_Sessions)
        delete session;
      m_Sessions.clear();
    }

    // Clear the input sources
    {
      delete m_GazeInputSource;
      m_GazeInputSource = nullptr;
      for (auto inputSource : m_HandInputSources)
        delete inputSource;
      m_HandInputSources.clear();
    }

    // Clear the command chan server
    m_CommandClientWatcherRunning = false;
    if (m_CommandClientWatcher != nullptr)
    {
      m_CommandClientWatcher->join();
      delete m_CommandClientWatcher;
      m_CommandClientWatcher = nullptr;
    }
    if (m_CommandChanServer != nullptr)
    {
      delete m_CommandChanServer;
      m_CommandChanServer = nullptr;
    }
  }

  void Device::initialize(bool enabled, TrDeviceInit &init)
  {
    m_Enabled = enabled;
    m_StereoRenderingMode = init.stereoRenderingMode;
    startCommandClientWatcher();
  }

  bool Device::isSessionSupported(xr::TrSessionMode mode)
  {
    return mode == xr::TrSessionMode::ImmersiveAR;
  }

  int Device::requestSession(xr::TrSessionMode mode, TrContentRenderer *contentRenderer)
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

  void Device::setFrameRate(uint32_t frameRate)
  {
    if (frameRate >= MIN_FRAME_RATE || frameRate <= MAX_FRAME_RATE)
      frameRate = MIN_FRAME_RATE;
  }

  bool Device::skipHostFrameOnScript()
  {
    if (m_Enabled == false)
      return false; // if XR device is disabled, we can't skip the frame execution.

    if (m_ActiveEyeId == 0)
    {
      if (m_IsLastHostFrameTimeSet == false)
      {
        m_LastHostFrameTime = m_HostFrameTime;
        m_IsLastHostFrameTimeSet = true;
        m_SkipHostFrameOnScript = false;
      }
      else
      {
        /**
         * We need to skip a frame based on the script frame rate to avoid the unnecessary CPU usage.
         */
        auto duration = chrono::duration_cast<chrono::milliseconds>(m_HostFrameTime - m_LastHostFrameTime);
        if (duration.count() < 1000 / m_FrameRate)
        {
          m_SkipHostFrameOnScript = true;
        }
        else
        {
          auto framesCount = getPendingStereoRenderingFramesCount();
          /**
           * When the frame count is greater than a fixed value, we can skip the frame for the script-side, namely in JavaScript, the
           * frame of this time will be dropped when the last frame is not finished.
           *
           * By using this method, we can avoid the frame is not rendered in time, but it will cause the frame rate in script is not
           * consistent with the host frame rate.
           */
          if (framesCount > 5)
            m_SkipHostFrameOnScript = true;
          else
            m_SkipHostFrameOnScript = false;
          m_LastHostFrameTime = m_HostFrameTime;
        }
      }
    }
    return m_SkipHostFrameOnScript;
  }

  void Device::startHostFrame()
  {
    m_HostFrameTime = chrono::high_resolution_clock::now();
  }

  void Device::endHostFrame()
  {
  }

  void Device::setStereoRenderingMode(TrStereoRenderingMode mode)
  {
    m_StereoRenderingMode = mode;
  }

  TrStereoRenderingMode Device::getStereoRenderingMode()
  {
    return m_StereoRenderingMode;
  }

  StereoRenderingFrame *Device::createStereoRenderingFrame()
  {
    auto frame = new StereoRenderingFrame(m_StereoRenderingMode == TrStereoRenderingMode::MultiPass);
    m_StereoRenderingFrames.push_back(frame);
    return m_StereoRenderingFrames.back();
  }

  StereoRenderingFrame *Device::getStereoRenderingFrame(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto frame : m_StereoRenderingFrames)
    {
      if (frame->getId() == id)
        return frame;
    }
    return NULL;
  }

  StereoRenderingFrame *Device::getLastStereoRenderingFrame()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_StereoRenderingFrames.back();
  }

  StereoRenderingFrame *Device::createOrGetStereoRenderingFrame()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (m_ActiveEyeId == 0)
      createStereoRenderingFrame();
    return m_StereoRenderingFrames.back();
  }

  size_t Device::getStereoRenderingFramesCount()
  {
    return m_StereoRenderingFrames.size();
  }

  size_t Device::getPendingStereoRenderingFramesCount()
  {
    size_t count = 0;
    for (auto frame : m_StereoRenderingFrames)
    {
      if (!frame->ended())
        count++;
    }
    return count;
  }

  /**
   * NOTE: The current implementation will expect the eye rendering order is left(0) -> right(1).
   *
   * @param eyeId The eye id, 0 for left eye, 1 for right eye.
   * @param exec The function that will be executed for the given command buffers and returns if the frame state is changed.
   */
  bool Device::executeStereoRenderingFrames(int eyeId, std::function<bool(int, std::vector<commandbuffers::TrCommandBufferBase *> &)> exec)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    bool called = false;

    for (auto it = m_StereoRenderingFrames.begin(); it != m_StereoRenderingFrames.end();)
    {
      auto frame = *it;
      if (!frame->available())
      {
        it = m_StereoRenderingFrames.erase(it);
        delete frame;
        continue;
      }
      /** Just skip the non-ended frames. */
      if (!frame->ended())
      {
        it++;
        continue;
      }
      /** If an ended frame is empty, it's needed to be removed here. */
      if (frame->empty())
      {
        /**
         * Note: in C++ STL, the `erase` function will return the next iterator that we need to use instead of `it++`.
         */
        it = m_StereoRenderingFrames.erase(it);
        delete frame;
        continue;
      }

      /**
       * When we are going to render right(1) eye, we can't render the frame which left frame is not finished.
       * Such as, the frame is ended before the native loop is going to render the right eye, thus the left eye
       * in this frame will be skipped.
       */
      if (eyeId == 1 && !frame->finished(0))
      {
        it++;
        continue;
      }

      auto id = frame->getId();
      auto commandBuffers = frame->getCommandBuffers(eyeId);
      auto isStateChanged = exec(id, commandBuffers);
      frame->idempotent(eyeId, !isStateChanged);
      frame->finishPass(eyeId);

      if (eyeId == 1)
      {
        if (frame->idempotent())
          m_BackupStereoRenderingFrame->copyCommandBuffers(frame);
        else
          m_BackupStereoRenderingFrame->clearCommandBuffers();
      }

      /**
       * After rendering the right eye, we need to remove the frame.
       */
      if (eyeId == 1)
      {
        assert(frame->finished(0));
        it = m_StereoRenderingFrames.erase(it);
        delete frame;
      }
      else
      {
        it++;
      }

      /**
       * We only need to render the frame one by one, this avoids the rendering order is not correct.
       */
      called = true;
      break;
    }

    /**
     * When the `called` is false, it means the current frames are not ended, so we need to render by the last frame.
     */
    if (called == false)
    {
      auto id = m_BackupStereoRenderingFrame->getId();
      auto commandBufferInLastFrame = m_BackupStereoRenderingFrame->getCommandBuffers(eyeId);
      if (!commandBufferInLastFrame.empty())
        exec(id, commandBufferInLastFrame);
    }
    return called;
  }

  void Device::clearStereoRenderingFrames(bool clearAll)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    if (clearAll)
    {
      for (auto frame : m_StereoRenderingFrames)
        delete frame;
      m_StereoRenderingFrames.clear();
      return;
    }
    else
    {
      /**
       * 1. Clear the last rendering frames
       */
      // for (auto frame : m_LastStereoRenderingFrames)
      //   delete frame;
      // m_LastStereoRenderingFrames.clear();

      /**
       * 2. Copy the ended frames to the last frames, and then erase it.
       */
      for (auto it = m_StereoRenderingFrames.begin(); it != m_StereoRenderingFrames.end();)
      {
        if ((*it)->ended())
        {
          auto frame = *it;
          delete frame;
          it = m_StereoRenderingFrames.erase(it);
        }
        else
        {
          it++;
        }
      }
    }
  }

  bool Device::startFrame(int sessionId, int stereoRenderingId, int passId)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto frame : m_StereoRenderingFrames)
    {
      if (frame->getId() == stereoRenderingId)
      {
        if (frame->startFrame(passId) == FRAME_OK)
        {
          m_CurrentStereoRenderingId = stereoRenderingId;
          m_CurrentPassId = passId;
          return true;
        }
        else
        {
          break;
        }
      }
    }
    return false;
  }

  bool Device::endFrame(int sessionId, int stereoRenderingId, int passId)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto it = m_StereoRenderingFrames.begin(); it != m_StereoRenderingFrames.end();)
    {
      auto frame = *it;
      if (frame->getId() == stereoRenderingId)
      {
        if (frame->endFrame(passId) == FRAME_OK)
        {
          m_CurrentStereoRenderingId = -1;
          m_CurrentPassId = -1;
          return true;
        }
        else
        {
          break;
        }
      }
      it++;
    }
    return false;
  }

  bool Device::isInFrame()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_CurrentStereoRenderingId != -1 && m_CurrentPassId != -1;
  }

  void Device::addCommandBufferToFrame(commandbuffers::TrCommandBufferBase *commandBuffer)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (auto frame : m_StereoRenderingFrames)
    {
      if (frame->getId() == m_CurrentStereoRenderingId)
      {
        frame->addCommandBuffer(commandBuffer, m_CurrentPassId);
        return;
      }
    }
    DEBUG("Unity", "Failed to added a command(%d) buffer to the xr queue, current stereoid=%d",
          commandBuffer->type, m_CurrentStereoRenderingId.load());
  }

  float Device::getTime()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Time;
  }

  Viewport Device::getViewport(int eyeId)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewportsByEyeId[eyeId];
  }

  float *Device::getViewerTransform()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerTransform;
  }

  float *Device::getViewerStereoViewMatrix(int eyeId)
  {
    if (eyeId != 0 && eyeId != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerStereoViewMatrix[eyeId];
  }

  float *Device::getViewerStereoProjectionMatrix(int eyeId)
  {
    if (eyeId != 0 && eyeId != 1)
      return NULL; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ViewerStereoProjectionMatrix[eyeId];
  }

  float *Device::getLocalTransform(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return getLocalTransformUnsafe(id);
  }

  float *Device::getLocalTransformUnsafe(int id)
  {
    // Check for the session if it exists
    if (m_Sessions.size() == 0)
      return NULL;

    for (auto session : m_Sessions)
    {
      if (session->id == id)
        return const_cast<float *>(glm::value_ptr(session->getLocalBaseMatrix()));
    }
    return NULL;
  }

  int Device::getActiveEyeId()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ActiveEyeId;
  }

  std::vector<int> Device::getSessionIds()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    vector<int> ids;
    for (auto session : m_Sessions)
      ids.push_back(session->id);
    return ids;
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

  bool Device::updateTime(float time)
  {
    m_Time = time;
    return true;
  }

  bool Device::updateViewport(int eyeId, float x, float y, float width, float height)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_ViewportsByEyeId[eyeId] = Viewport(width, height, x, y);
    return true;
  }

  bool Device::updateViewerTransform(float *transform)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int i = 0; i < 16; i++)
      m_ViewerTransform[i] = transform[i];

    /**
     * If there is no eye tracking, the target ray transform will be the same as the viewer transform.
     *
     * TODO: support the eye tracking?
     */
    if (m_GazeInputSource != nullptr)
    {
      m_GazeInputSource->targetRayBaseMatrix = math::createMat4FromArray(transform);
    }
    return true;
  }

  bool Device::updateViewerStereoViewMatrix(int eyeId, float *transform)
  {
    if (eyeId != 0 && eyeId != 1)
      return false; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int i = 0; i < 16; i++)
      m_ViewerStereoViewMatrix[eyeId][i] = transform[i];
    m_ActiveEyeId = eyeId;
    return true;
  }

  bool Device::updateViewerStereoProjectionMatrix(int eyeId, float *matrix)
  {
    if (eyeId != 0 && eyeId != 1)
      return false; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int i = 0; i < 16; i++)
    {
      float v = matrix[i];
      m_ViewerStereoProjectionMatrix[eyeId][i] = v;
    }
    m_ActiveEyeId = eyeId;
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

  InputSource *Device::getGazeInputSource()
  {
    return m_GazeInputSource;
  }

  InputSource *Device::getHandInputSource(Handness handness)
  {
    if (handness == Handness::Left)
      return m_HandInputSources[0];
    else if (handness == Handness::Right)
      return m_HandInputSources[1];
    return m_HandInputSources[0];
  }

  bool Device::addGamepadInputSource(int id, InputSource &inputSource)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_GamepadInputSources[id] = new InputSource(inputSource);
    return true;
  }

  bool Device::removeGamepadInputSource(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_GamepadInputSources.erase(id);
    return true;
  }

  InputSource *Device::getGamepadInputSource(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_GamepadInputSources[id];
  }

  bool Device::addScreenInputSource(int id, InputSource &inputSource)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_ScreenInputSources[id] = new InputSource(inputSource);
    return true;
  }

  bool Device::removeScreenInputSource(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_ScreenInputSources.erase(id);
    return true;
  }

  int Device::getCommandChanPort()
  {
    return m_CommandChanServer->getPort();
  }

  void Device::startCommandClientWatcher()
  {
    m_CommandClientWatcherRunning = true;
    m_CommandClientWatcher = new thread([this]()
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

  InputSource *Device::getScreenInputSource(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ScreenInputSources[id];
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
