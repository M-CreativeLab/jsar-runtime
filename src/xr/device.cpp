#include <assert.h>
#include "device.hpp"
#include "math/matrix.hpp"

namespace xr
{
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
    m_LastStereoRenderingFrame = new StereoRenderingFrame(true);
  }

  Device::~Device()
  {
    m_FieldOfView = 0.0f;
    m_Time = 0.0f;
    m_SessionIds.clear();
    delete m_LastStereoRenderingFrame;
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
    std::lock_guard<std::mutex> lock(m_Mutex);
    auto isMultiPass = m_StereoRenderingMode == StereoRenderingMode::MultiPass;
    m_StereoRenderingFrames.push_back(new StereoRenderingFrame(isMultiPass));
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

  bool Device::executeStereoRenderingFrames(int eyeId, std::function<bool(std::vector<renderer::CommandBuffer *> &)> exec)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    bool called = false;

    for (auto frame : m_StereoRenderingFrames)
    {
      if (!frame->ended())
        continue;

      auto commandBuffers = frame->getCommandBuffers(eyeId);
      if (exec(commandBuffers))
      {
        m_LastStereoRenderingFrame->copyCommandBuffers(commandBuffers, eyeId);
        if (!called)
          called = true;
      }
    }

    /**
     * When the `called` is false, it means the current frames are not ended, so we need to render by the last frame.
     */
    if (called == false)
    {
      auto commandBufferInLastFrame = m_LastStereoRenderingFrame->getCommandBuffers(eyeId);
      called = exec(commandBufferInLastFrame);
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
    auto rightHanded = math::ConvertMatrixToRightHanded(input);
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
}
