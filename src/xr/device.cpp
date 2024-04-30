#include <assert.h>
#include <algorithm>
#include "device.hpp"
#include "math/matrix.hpp"

namespace xr
{
  static uint32_t MIN_FRAME_RATE = 60;
  static uint32_t MAX_FRAME_RATE = 90;
  Device *Device::s_instance = NULL;

  Device *Device::GetInstance()
  {
    return s_instance;
  }

  Device *Device::Create()
  {
    assert(s_instance == NULL);
    s_instance = new Device();
    return s_instance;
  }

  void Device::Destroy()
  {
    if (s_instance == NULL)
      return;
    delete s_instance;
    s_instance = NULL;
  }

  Device::Device() : m_FieldOfView(0.0f), m_Time(0.0f)
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
  }

  Device::~Device()
  {
    m_FieldOfView = 0.0f;
    m_Time = 0.0f;
    m_SessionIds.clear();
    delete m_BackupStereoRenderingFrame;
    m_BackupStereoRenderingFrame = nullptr;

    // Clear the input sources
    {
      delete m_GazeInputSource;
      m_GazeInputSource = nullptr;
      for (auto inputSource : m_HandInputSources)
        delete inputSource;
      m_HandInputSources.clear();
    }
  }

  void Device::initialize(bool enabled)
  {
    m_Enabled = enabled;
  }

  bool Device::requestSession(int id)
  {
    // Search for the session id, if the session id has been added, return false
    for (auto sessionId : m_SessionIds)
    {
      if (sessionId == id)
        return false;
    }
    m_SessionIds.push_back(id);
    DEBUG("Unity", "Device::requestSession(%d) finished", id);
    return true;
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

  void Device::setStereoRenderingMode(StereoRenderingMode mode)
  {
    m_StereoRenderingMode = mode;
  }

  StereoRenderingMode Device::getStereoRenderingMode()
  {
    return m_StereoRenderingMode;
  }

  StereoRenderingFrame *Device::createStereoRenderingFrame()
  {
    auto frame = new StereoRenderingFrame(m_StereoRenderingMode == StereoRenderingMode::MultiPass);
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
   */
  bool Device::executeStereoRenderingFrames(int eyeId, std::function<bool(int, std::vector<renderer::CommandBuffer *> &)> exec)
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
      exec(id, commandBuffers);
      frame->finishPass(eyeId);

      bool copyNotAllowed = false;
      // TODO: optimize the performance here
      // We could move this to the exec() function to avoid this loop.
      for (auto commandBuffer : commandBuffers)
      {
        switch (commandBuffer->GetType())
        {
        case renderer::kCommandTypeCreateBuffer:
        case renderer::kCommandTypeCreateVertexArray:
        case renderer::kCommandTypeCreateTexture:
        case renderer::kCommandTypeCreateSampler:
        case renderer::kCommandTypeCreateShader:
        case renderer::kCommandTypeLinkProgram:
          copyNotAllowed = true;
          break;
        default:
          break;
        }
        if (copyNotAllowed)
          break;
      }
      if (!copyNotAllowed)
        m_BackupStereoRenderingFrame->copyCommandBuffers(commandBuffers, eyeId);

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

  void Device::addCommandBufferToFrame(renderer::CommandBuffer *commandBuffer)
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
          commandBuffer->GetType(), m_CurrentStereoRenderingId.load());
  }

  void Device::onXRFrame(DeviceFrame *frame)
  {
    auto jsDeviceNative = bindings::XRDeviceNative::GetInstance();
    if (jsDeviceNative == NULL)
      return;
    jsDeviceNative->onFrame(frame);
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
    if (m_SessionIds.size() == 0)
      return NULL;

    for (auto sessionId : m_SessionIds)
    {
      if (sessionId == id)
        return m_LocalTransforms[id];
    }
    return NULL;
  }

  int Device::getActiveEyeId()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ActiveEyeId;
  }

  std::vector<int> &Device::getSessionIds()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_SessionIds;
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
    m_ViewportsByEyeId[eyeId] = Viewport(x, y, width, height);
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
    if (m_SessionIds.size() == 0)
      return false;

    std::array<float, 16> input = {
        transform[0], transform[1], transform[2], transform[3],
        transform[4], transform[5], transform[6], transform[7],
        transform[8], transform[9], transform[10], transform[11],
        transform[12], transform[13], transform[14], transform[15]};
    // auto rightHanded = math::ConvertMatrixToRightHanded(input);
    for (auto sessionId : m_SessionIds)
    {
      if (sessionId == id)
      {
        for (int i = 0; i < 16; i++)
          m_LocalTransforms[id][i] = input[i];
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

  InputSource *Device::getScreenInputSource(int id)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_ScreenInputSources[id];
  }
}
