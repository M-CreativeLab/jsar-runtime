#pragma once

#include <map>
#include <vector>
#include <mutex>

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
    float *getViewerStereoViewMatrix(int eyeId);
    float *getViewerStereoProjectionMatrix(int eyeId);
    float *getLocalTransform(int id);

  public:
    bool updateFov(float fov);
    bool updateTime(float time);
    bool updateViewerTransform(float *transform);
    bool updateViewerStereoViewMatrix(int eyeId, float *transform);
    bool updateViewerStereoProjectionMatrix(int eyeId, float *transform);
    bool updateLocalTransform(int id, float *transform);

  private:
    /**
     * Recommanded field of view.
     */
    float m_FieldOfView;
    /**
     * The timestamp in milliseconds for current frame.
     */
    float m_Time;
    /**
     * The viewer(camera or eyes) transform matrix, namely the viewer's model matrix, it's used to describe how to
     * transform the viewer's model to the world space.
     */
    float m_ViewerTransform[16];
    /**
     * The viewer(camera or eyes) view matrix.
     */
    map<int, float[16]> m_ViewerStereoViewMatrix;
    /**
     * The viewer(camera or eyes) projection matrix.
     */
    map<int, float[16]> m_ViewerStereoProjectionMatrix;
    /**
     * The local(object) transform matrix.
     */
    map<int, float[16]> m_LocalTransforms;
    /**
     * The id to indentify the session, corresponding to the session's id in the WebXR API.
     */
    vector<int> m_SessionIds;
    /**
     * A mutex to ensure the above data is thread-safe.
     */
    mutex m_Mutex;

  private:
    static Device *s_instance;
  };
} // namespace xr
