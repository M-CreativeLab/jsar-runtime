#pragma once

#include <array>
#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include "frame.hpp"

using namespace std;

namespace xr
{
  enum StereoRenderingMode
  {
    MultiPass = 0,
    SinglePass = 1,
    SinglePassInstanced = 2,
    SinglePassMultiview = 3,
    Unknown = -1
  };

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
    void initialize(bool enabled);
    bool requestSession(int id);
    bool enabled();
    void setStereoRenderingMode(StereoRenderingMode mode);
    StereoRenderingMode getStereoRenderingMode();
    StereoRenderingFrame *createStereoRenderingFrame();
    StereoRenderingFrame *getStereoRenderingFrame(int id);
    StereoRenderingFrame *getLastStereoRenderingFrame();
    /**
     * Iterate ended stereo rendering frames, it means the callback will not be called for the frames that are not ended.
     * And it returns true if the callback is called at least once, otherwise false.
     */
    bool iterateStereoRenderingFrames(function<void(StereoRenderingFrame *)> callback);
    void clearStereoRenderingFrames(bool clearAll = false);
    bool startFrame(int sessionId, int stereoRenderingId, int passId);
    bool endFrame(int sessionId, int stereoRenderingId, int passId);
    bool isInFrame();
    void addCommandBufferToFrame(renderer::CommandBuffer *commandBuffer);

  public:
    float getTime();
    float *getViewerTransform();
    float *getViewerStereoViewMatrix(int eyeId);
    float *getViewerStereoProjectionMatrix(int eyeId);
    float *getLocalTransform(int id);
    int getActiveEyeId();
    vector<int> &getSessionIds();

  public:
    bool updateFov(float fov);
    bool updateTime(float time);
    bool updateViewerTransform(float *transform);
    bool updateViewerStereoViewMatrix(int eyeId, float *transform);
    bool updateViewerStereoProjectionMatrix(int eyeId, float *transform);
    bool updateLocalTransform(int id, float *transform);

  private:
    bool m_Enabled = false;
    /**
     * Recommanded field of view.
     */
    atomic<float> m_FieldOfView;
    /**
     * The timestamp in milliseconds for current frame.
     */
    atomic<float> m_Time;
    /**
     * The stereo rendering mode, it affects how to use frame callback and execute frame.
     */
    atomic<StereoRenderingMode> m_StereoRenderingMode = StereoRenderingMode::Unknown;
    /**
     * Current stereo rendering frames.
     */
    atomic<int> m_CurrentStereoRenderingId = -1;
    atomic<int> m_CurrentPassId = -1;
    vector<StereoRenderingFrame *> m_StereoRenderingFrames;
    vector<StereoRenderingFrame *> m_LastStereoRenderingFrames;
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
     * The active eye's id, 0 for left and 1 for right. It's used in multi-pass rendering only.
     */
    int m_ActiveEyeId;
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
