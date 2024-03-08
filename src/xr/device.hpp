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

  public:
    void initialize();
    bool requestSession(int id);
    float *getViewerTransform();
    float *getLocalTransform(int id);

  public:
    bool updateFov(float fov);
    bool updateTime(float time);
    bool updateViewerPose(float x, float y, float z, float qx, float qy, float qz, float qw);
    bool updateLocalPose(int id, float x, float y, float z, float qx, float qy, float qz, float qw);
    bool updateViewerTransform(float *transform);
    bool updateLocalTransform(int id, float *transform);

  private:
    float m_FieldOfView;
    float m_Time;
    float m_ViewerPosition[3];
    float m_ViewerRotation[4];
    float m_ViewerTransform[16];
    map<int, float[3]> m_LocalPositions;
    map<int, float[4]> m_LocalRotations;
    map<int, float[16]> m_LocalTransforms;
    vector<int> m_SessionIds;

  private:
    static Device *s_instance;
  };
} // namespace xr
