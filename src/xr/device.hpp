#pragma once

#include <map>
#include <vector>

using namespace std;

namespace xr
{
  class Device
  {
  public:
    static Device *GetInstance();
    static Device *Create();
    static void Destroy();

  public:
    Device();
    ~Device();

    void initialize();
    bool requestSession(int id);

    bool updateFov(float fov);
    bool updateTime(float time);
    bool updateViewerPose(float x, float y, float z, float qx, float qy, float qz, float qw);
    bool updateLocalPose(int id, float x, float y, float z, float qx, float qy, float qz, float qw);

  private:
    float m_FieldOfView;
    float m_Time;
    float m_ViewerPosition[3];
    float m_ViewerRotation[4];
    map<int, float[3]> m_LocalPositions;
    map<int, float[4]> m_LocalRotations;
    vector<int> m_SessionIds;

  private:
    static Device *s_instance;
  };
} // namespace xr
