#pragma once

#include <array>
#include <map>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <chrono>

#include "common/classes.hpp"
#include "common/ipc.hpp"
#include "common/zone.hpp"
#include "common/xr/types.hpp"
#include "common/xr/message.hpp"
#include "common/collision/ray.hpp"
#include "common/math3d/plane.hpp"
#include "common/math3d/frustum.hpp"
#include "common/command_buffers/command_buffers.hpp"

#include "./session.hpp"
#include "./frame.hpp"
#include "./viewport.hpp"
#include "./input_source.hpp"

using namespace std;

namespace xr
{
  /**
   * The XR Device is used to manage XR sessions, input sources and other XR resources.
   */
  class Device
  {
  public:
    static Device *GetInstance();

  public:
    Device(TrConstellation *constellation);
    ~Device();

  public:
    /**
     * Configure the XR Device with a specific `TrDeviceInit` configuration.
     * 
     * @param init the configuration init to configure the device.
     */
    void configure(TrDeviceInit &init);
    /**
     * Initialize the XR device.
     */
    void initialize();
    /**
     * Shutdown the XR device.
     */
    void shutdown();
    /**
     * It returns if this session mode (immersive-ar, immersive-vr or inline) is supported by the XR device.
     */
    bool isSessionSupported(xr::TrXRSessionMode mode);
    /**
     * It requests a XR session with its mode and content source.
     */
    int requestSession(xr::TrXRSessionMode mode, TrContentRenderer *contentRenderer);
    /**
     * It ends and removes a session by its id.
     */
    bool endAndRemoveSession(xr::TrXRSession *session);
    /**
     * If this device is enabled.
     */
    bool enabled();
    /**
     * If this device is rendering as multipass.
     */
    bool isRenderedAsMultipass();
    /**
     * It returns the stereo rendering mode.
     */
    TrStereoRenderingMode getStereoRenderingMode();
    /**
     * It creates a new stereo rendering frame.
     *
     * @returns a new instance of StereoRenderingFrame.
     */
    StereoRenderingFrame *createStereoRenderingFrame();

  public:
    /**
     * It returns the viewport for the specific view index.
     */
    Viewport getViewport(int viewIndex);
    /**
     * It returns the XR viewer/camera's base matrix.
     */
    float *getViewerBaseMatrix();
    /**
     * It returns the view matrix for eye.
     */
    float *getViewMatrixForEye(int eye);
    /**
     * It returns the projection matrix for eye.
     */
    float *getProjectionMatrixForEye(int eye);
    glm::mat4 getLocalTransform(int id);
    glm::mat4 getLocalTransformUnsafe(int id);
    /**
     * It returns the current rendering eye or view index.
     */
    int getActiveEyeId();

  public: // Sessions
    void iterateSessionsByContentPid(pid_t contentPid, function<void(TrXRSession *)> callback);

  public:
    bool updateFov(float fov);
    /**
     * Update current framebuffer's viewport.
     */
    bool updateViewport(int eyeId, float x, float y, float width, float height);
    /**
     * Update the viewer's base matrix.
     */
    bool updateViewerBaseMatrix(float *baseMatrixValues);
    /**
     * Update the view matrix for a view/eye.
     *
     * @param viewIndex the view index to update data.
     * @param viewMatrixValues the float array to be updated.
     */
    bool updateViewMatrix(int viewIndex, float *viewMatrixValues);
    /**
     * Update the projection matrix for a view/eye.
     *
     * @param viewIndex the view index to update data.
     * @param projectionMatrixValues the float array to be updated.
     */
    bool updateProjectionMatrix(int viewIndex, float *projectionMatrixValues);
    /**
     * Update the local transform by the session id.
     * 
     * @param sessionId the session id to find the session.
     * @param transform the float array to be updated.
     * @returns true if the local transform is updated successfully.
     */
    bool updateLocalTransformBySessionId(int sessionId, float *transform);
    /**
     * Update the local transform by the document id, it will find the session by the document/content's active session id.
     * 
     * @param documentId the document id to find the session.
     * @param transform the float array to be updated.
     * @returns true if the local transform is updated successfully.
     */
    bool updateLocalTransformByDocumentId(int documentId, float *transform);
    /**
     * Update the gaze-related data from the eyeball or camera's base matrix.
     */
    void updateGazeFromBaseMatrix(float *baseMatrixValues)
    {
      // Update the input source for gaze.
      if (m_InputSourcesZone != nullptr)
      {
        auto gazeInputSource = m_InputSourcesZone->getGazeInputSource();
        gazeInputSource->setTargetRayBaseMatrix(baseMatrixValues);
      }

      // Update the target ray for gaze.
      auto baseMatrix = glm::make_mat4(baseMatrixValues);
      auto origin = glm::vec3(glm::column(baseMatrix, 3));
      auto forward = glm::normalize(glm::vec3(-glm::column(baseMatrix, 2)));
      m_GazeRay.update(origin, forward);
    }

  public: // Input sources
    string getInputSourcesZonePath();
    void syncInputSourcesToZone();
    TrXRInputSource *getGazeInputSource();
    TrXRInputSource *getHandInputSource(int id);
    TrXRInputSource *getHandInputSource(TrHandness handness);
    TrXRInputSource *getGamepadInputSource(int id);
    TrXRInputSource *getScreenInputSource(int id);
    void addGamepadInputSource(TrXRInputSource &newInputSource);
    void removeGamepadInputSource(int id);
    void addScreenInputSource(TrXRInputSource &newInputSource);
    void removeScreenInputSource(int id);

  public: // Command channel
    int getCommandChanPort();
    void startCommandClientWatcher();
    void handleCommandMessage(TrXRCommandMessage &message, TrContentRuntime *content);

  private: // XR command channel handlers
    xr::IsSessionSupportedResponse onIsSessionSupportedRequest(xr::IsSessionSupportedRequest &request, TrContentRenderer *contentRenderer);
    xr::SessionResponse onSessionRequest(xr::SessionRequest &request, TrContentRenderer *contentRenderer);

  private:
    /**
     * The constellation object.
     */
    TrConstellation *m_Constellation = nullptr;
    /**
     * A flag to indicate if the XR is enabled.
     */
    bool m_Enabled = false;
    /**
     * Recommanded field of view.
     */
    atomic<float> m_FieldOfView;
    /**
     * The stereo rendering mode, it affects how to use frame callback and execute frame.
     */
    atomic<TrStereoRenderingMode> m_StereoRenderingMode = TrStereoRenderingMode::Unknown;
    /**
     * The viewport for each view.
     */
    map<int, Viewport> m_ViewportsByEyeId;
    /**
     * The viewer(camera) base matrix.
     */
    float m_ViewerBaseMatrix[16];
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
    atomic<int> m_ActiveEyeId;
    /**
     * The id to indentify the session, corresponding to the session's id in the WebXR API.
     */
    vector<TrXRSession *> m_Sessions;
    /**
     * Input sources fields
     */
    std::unique_ptr<TrXRInputSourcesZone> m_InputSourcesZone;
    /**
     * The native ray for gaze, it's used to detect which session's content is gazed or loss focus.
     */
    collision::TrRay m_GazeRay;
    std::array<math3d::TrPlane, 6> m_ViewerFrustumPlanes;
    /**
     * A mutex to ensure the above data is thread-safe.
     */
    shared_mutex m_MutexForSessions;
    shared_mutex m_MutexForValueUpdates;

  private: // command channel
    std::unique_ptr<ipc::TrOneShotServer<TrXRCommandMessage>> m_CommandChanServer = nullptr;
    std::unique_ptr<thread> m_CommandClientWatcher = nullptr;
    atomic<bool> m_CommandClientWatcherRunning = false;
    int m_AcceptTimeout = 1000;

    friend class TrXRSession;
  };
} // namespace xr
