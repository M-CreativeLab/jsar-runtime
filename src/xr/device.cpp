#include <assert.h>
#include "device.hpp"

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
  }

  Device::~Device()
  {
    m_FieldOfView = 0.0f;
    m_Time = 0.0f;
    m_SessionIds.clear();
  }

  void Device::initialize(bool enabled)
  {
    m_Enabled = enabled;
  }

  void Device::setStereoRenderingMode(StereoRenderingMode mode)
  {
    m_StereoRenderingMode = mode;
  }

  StereoRenderingMode Device::getStereoRenderingMode()
  {
    return m_StereoRenderingMode;
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
    return true;
  }

  bool Device::enabled()
  {
    return m_Enabled;
  }

  float Device::getTime()
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_Time;
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

  bool Device::updateViewerStereoProjectionMatrix(int eyeId, float *transform)
  {
    if (eyeId != 0 && eyeId != 1)
      return false; // Invalid eye id (0 or 1)

    std::lock_guard<std::mutex> lock(m_Mutex);
    for (int i = 0; i < 16; i++)
      m_ViewerStereoProjectionMatrix[eyeId][i] = transform[i];
    m_ActiveEyeId = eyeId;
    return true;
  }

  bool Device::updateLocalTransform(int id, float *transform)
  {
    std::lock_guard<std::mutex> lock(m_Mutex);
    // Check for the session if it exists
    if (m_SessionIds.size() == 0)
      return false;

    for (auto sessionId : m_SessionIds)
    {
      if (sessionId == id)
      {
        for (int i = 0; i < 16; i++)
          m_LocalTransforms[id][i] = transform[i];
        return true;
      }
    }
    return false;
  }
}
