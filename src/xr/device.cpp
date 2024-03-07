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
    m_ViewerPosition[0] = 0.0f;
    m_ViewerPosition[1] = 0.0f;
    m_ViewerPosition[2] = 0.0f;
    m_ViewerRotation[0] = 0.0f;
    m_LocalPositions.clear();
    m_LocalRotations.clear();
    m_SessionIds.clear();
  }

  void Device::initialize()
  {
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

  bool Device::updateViewerPose(float x, float y, float z, float qx, float qy, float qz, float qw)
  {
    m_ViewerPosition[0] = x;
    m_ViewerPosition[1] = y;
    m_ViewerPosition[2] = z;
    m_ViewerRotation[0] = qx;
    m_ViewerRotation[1] = qy;
    m_ViewerRotation[2] = qz;
    m_ViewerRotation[3] = qw;
    return true;
  }

  bool Device::updateLocalPose(int sessionId, float x, float y, float z, float qx, float qy, float qz, float qw)
  {
    // Check for the session if it exists
    if (m_SessionIds.size() == 0)
      return false;

    for (auto id : m_SessionIds)
    {
      if (id == sessionId)
      {
        m_LocalPositions[id][0] = x;
        m_LocalPositions[id][1] = y;
        m_LocalPositions[id][2] = z;
        m_LocalRotations[id][0] = qx;
        m_LocalRotations[id][1] = qy;
        m_LocalRotations[id][2] = qz;
        m_LocalRotations[id][3] = qw;
        return true;
      }
    }
    return false;
  }
}
