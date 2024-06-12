#pragma once

#include <array>
#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>

#include "common/ipc.hpp"
#include "common/xr/types.hpp"
#include "common/xr/message.hpp"
#include "common/command_buffers/command_buffers.hpp"

#include "./frame.hpp"
#include "./viewport.hpp"
#include "./input_source.hpp"

using namespace std;

namespace xr
{
  class Device
  {
  public:
    static Device *GetInstance();

  public:
    Device();
    ~Device();

  public:
    void initialize(bool enabled, TrDeviceInit &init);
    bool requestSession(int id);
    bool enabled();
    void setFrameRate(uint32_t frameRate);
    bool skipHostFrameOnScript();
    void startHostFrame();
    void endHostFrame();

    void setStereoRenderingMode(TrStereoRenderingMode mode);
    TrStereoRenderingMode getStereoRenderingMode();

    StereoRenderingFrame *createStereoRenderingFrame();
    StereoRenderingFrame *getStereoRenderingFrame(int id);
    StereoRenderingFrame *getLastStereoRenderingFrame();
    StereoRenderingFrame *createOrGetStereoRenderingFrame();

    size_t getStereoRenderingFramesCount();
    size_t getPendingStereoRenderingFramesCount();
    bool executeStereoRenderingFrames(int eyeId, function<bool(int, vector<commandbuffers::TrCommandBufferBase *> &)>);
    void clearStereoRenderingFrames(bool clearAll = false);
    bool startFrame(int sessionId, int stereoRenderingId, int passId);
    bool endFrame(int sessionId, int stereoRenderingId, int passId);
    bool isInFrame();
    void addCommandBufferToFrame(commandbuffers::TrCommandBufferBase *commandBuffer);
    /**
     * Call the frame callback registered by the JavaScript side with the given `DeviceFrame`.
     */
    void onXRFrame(DeviceFrame *frame);

  public:
    float getTime();
    Viewport getViewport(int eyeId);
    float *getViewerTransform();
    float *getViewerStereoViewMatrix(int eyeId);
    float *getViewerStereoProjectionMatrix(int eyeId);
    float *getLocalTransform(int id);
    float *getLocalTransformUnsafe(int id);
    int getActiveEyeId();
    vector<int> &getSessionIds();

  public:
    bool updateFov(float fov);
    bool updateTime(float time);
    bool updateViewport(int eyeId, float x, float y, float width, float height);
    bool updateViewerTransform(float *transform);
    bool updateViewerStereoViewMatrix(int eyeId, float *transform);
    bool updateViewerStereoProjectionMatrix(int eyeId, float *transform);
    bool updateLocalTransform(int id, float *transform);

  public:
    /**
     * Input sources
     */
    InputSource *getGazeInputSource();
    InputSource *getHandInputSource(Handness handness);
    bool addGamepadInputSource(int id, InputSource &gamepadInputSource);
    InputSource *getGamepadInputSource(int id);
    bool removeGamepadInputSource(int id);
    bool addScreenInputSource(int id, InputSource &screenInputSource);
    InputSource *getScreenInputSource(int id);
    bool removeScreenInputSource(int id);

  public:
    int getCommandChanPort();

  private:
    /**
     * A flag to indicate if the XR is enabled.
     */
    bool m_Enabled = false;
    /**
     * If the host frame should be skipped for the script.
     *
     * In multi-pass rendering, this could be set by the left eye's frame callback, and used by both eyes, so that the
     * right eye's frame callback will not be called.
     */
    bool m_SkipHostFrameOnScript = false;
    bool m_IsLastHostFrameTimeSet = false;
    /**
     * A host frame is the frame called by the host environment, such as the Unity engine, the host frame is the Unity's
     * Update() method, and then the time of host frame is the time point when the host frame is called.
     */
    chrono::time_point<chrono::high_resolution_clock> m_HostFrameTime;
    chrono::time_point<chrono::high_resolution_clock> m_LastHostFrameTime;

    /**
     * The frame rate for the XR sessions.
     */
    uint32_t m_FrameRate = 60;

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
    atomic<TrStereoRenderingMode> m_StereoRenderingMode = TrStereoRenderingMode::Unknown;
    /**
     * Current stereo rendering frames.
     */
    atomic<int> m_CurrentStereoRenderingId = -1;
    atomic<int> m_CurrentPassId = -1;
    vector<StereoRenderingFrame *> m_StereoRenderingFrames;
    StereoRenderingFrame *m_BackupStereoRenderingFrame;
    /**
     * The viewport for each view.
     */
    map<int, Viewport> m_ViewportsByEyeId;
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
    /**
     * Input sources fields
     */
    // input source for gaze
    InputSource *m_GazeInputSource;
    // input sources(2) for hands
    std::vector<InputSource *> m_HandInputSources;
    std::map<int, InputSource *> m_ScreenInputSources;
    std::map<int, InputSource *> m_GamepadInputSources;

  private:
    ipc::TrOneShotServer<TrXRCommandMessage> *m_CommandChanServer = nullptr;
  };
} // namespace xr
