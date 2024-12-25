#pragma once

#include <array>
#include <map>
#include <memory>
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

namespace xr
{
  /**
   * The WebXR device is used to manage WebXR sessions, input sources and other resources.
   */
  class Device
  {
    friend class TrXRSession;

  public:
    /**
     * Create a new WebXR device instance.
     *
     * @param constellation The constellation instance.
     * @returns The new WebXR device instance.
     */
    static std::shared_ptr<Device> Make(TrConstellation *constellation)
    {
      return std::make_shared<Device>(constellation);
    }

  public:
    Device() = delete;
    Device(TrConstellation *constellation);
    ~Device() = default;

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
     * Tick function.
     */
    void tick();
    /**
     * It returns if this session mode (immersive-ar, immersive-vr or inline) is supported by the XR device.
     *
     * @param mode the session mode to check if it is supported.
     * @returns `true` if the session mode is supported.
     */
    bool isSessionSupported(TrXRSessionMode mode);
    /**
     * It requests a XR session with its mode and content source.
     *
     * @param mode the session mode to request the WebXR session.
     * @param content the content source to request the WebXR session.
     * @returns the new instance of `TrXRSession`.
     */
    std::shared_ptr<TrXRSession> requestSession(TrXRSessionMode mode, std::shared_ptr<TrContentRuntime> content);
    /**
     * It ends and removes a session by its id.
     *
     * @param session the session to end and remove.
     * @returns `true` if the session is ended and removed successfully.
     */
    bool endAndRemoveSession(TrXRSession *session);
    /**
     * @returns `true` if the device is enabled.
     */
    bool enabled();
    /**
     * @returns `true` if the device is rendering as multipass.
     */
    bool isRenderedAsMultipass();
    /**
     * @returns the mode for stereo rendering.
     */
    TrStereoRenderingMode getStereoRenderingMode();
    /**
     * It creates a new stereo rendering frame.
     *
     * @param stereoId the stereo id for the new stereo rendering frame.
     * @returns a new `StereoRenderingFrame` instance.
     */
    StereoRenderingFrame *createStereoRenderingFrame(int stereoId);

  public:
    /**
     * It returns the viewport for the specific view index.
     *
     * @param viewIndex the view index to get the viewport.
     * @returns the viewport for the specific view index.
     */
    Viewport getViewport(int viewIndex);
    /**
     * @returns the WebXR viewer base matrix.
     */
    float *getViewerBaseMatrix();
    /**
     * Get the view matrix for specific eye.
     *
     * @param eye the eye index to get the view matrix.
     * @returns the view matrix.
     */
    float *getViewMatrixForEye(int eye);
    /**
     * Get the projection matrix for eye.
     *
     * @param eye the eye index to get the projection matrix.
     * @returns the projection matrix.
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
     * Update the current view's framebuffer.
     *
     * @param framebufferId the framebuffer id to update.
     * @param viewport the viewport to update.
     * @param useDoubleWide if the framebuffer is double wide.
     */
    bool updateViewFramebuffer(int framebufferId, TrViewport viewport, bool useDoubleWide);
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
     * Get the collision box by the document id, it will find the session by the document/content's active session id.
     *
     * @param documentId the document id to find the session.
     * @param outMin the min vector to store the result.
     * @param outMax the max vector to store the result.
     * @returns true if the local bounding box is found.
     */
    bool getCollisionBoxByDocumentId(int documentId, float *outMin, float *outMax);
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
    /**
     * Get the session context zone directory.
     *
     * @returns the session context zone directory.
     */
    inline std::string getSessionContextZoneDirectory() { return m_SessionContextZoneDirectory; }
    /**
     * Get the device context zone path.
     *
     * @returns the device context zone path.
     */
    inline std::string getDeviceContextZonePath() { return m_DeviceContextZone->getFilename(); }

  public: // Input sources
    /**
     * Get the input sources zone path.
     *
     * @returns the input sources zone path.
     */
    std::string getInputSourcesZonePath();
    /**
     * Returns the gaze input source for updating fields.
     */
    TrXRInputSource *getGazeInputSource();
    /**
     * Returns the main controller input source for updating fields.
     */
    TrXRInputSource *getMainControllerInputSource();
    /**
     * Returns the hand input source by hand id: 0 for left and 1 for right.
     */
    TrXRInputSource *getHandInputSource(int id);
    /**
     * Returns the hand input source by handness.
     */
    TrXRInputSource *getHandInputSource(TrHandness handness);
    /**
     * Returns the screen controller input source.
     *
     * @param index the index of the screen controller.
     * @returns the screen controller input source.
     */
    TrXRInputSource *getScreenControllerInputSource(int index);
    /**
     * Get the input source by its id.
     *
     * @param id the input source id.
     * @returns the input source object.
     */
    TrXRInputSource *getInputSourceById(int id);

  public: // Command channel
    int getCommandChanPort();
    void startCommandClientWatcher();
    void handleCommandMessage(TrXRCommandMessage &message, std::shared_ptr<TrContentRuntime> content);

  private: // XR command channel handlers
    void onIsSessionSupportedRequest(xr::IsSessionSupportedRequest &request, std::shared_ptr<TrContentRuntime> content);
    void onSessionRequest(xr::SessionRequest &request, std::shared_ptr<TrContentRuntime> content);
    void onEndSessionRequest(xr::EndSessionRequest &request, std::shared_ptr<TrContentRuntime> content);

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
    std::atomic<float> m_FieldOfView;
    /**
     * The stereo rendering mode, it affects how to use frame callback and execute frame.
     */
    std::atomic<TrStereoRenderingMode> m_StereoRenderingMode = TrStereoRenderingMode::Unknown;
    /**
     * The viewport for each view.
     */
    std::map<int, Viewport> m_ViewportsByEyeId;
    /**
     * The viewer(camera) base matrix.
     */
    float m_ViewerBaseMatrix[16];
    /**
     * The viewer(camera or eyes) view matrix.
     */
    std::map<int, float[16]> m_ViewerStereoViewMatrix;
    /**
     * The viewer(camera or eyes) projection matrix.
     */
    std::map<int, float[16]> m_ViewerStereoProjectionMatrix;
    /**
     * The local(object) transform matrix.
     */
    std::map<int, float[16]> m_LocalTransforms;
    /**
     * The active eye's id, 0 for left and 1 for right. It's used in multi-pass rendering only.
     */
    std::atomic<int> m_ActiveEyeId;
    /**
     * The id to indentify the session, corresponding to the session's id in the WebXR API.
     */
    std::vector<std::shared_ptr<TrXRSession>> m_Sessions;
    /**
     * The session context zone directory for session-related shared data.
     */
    std::string m_SessionContextZoneDirectory;
    /**
     * The device context zone for device-related shared data.
     */
    std::unique_ptr<TrXRDeviceContextZone> m_DeviceContextZone;
    /**
     * The input sources zone for input-related shared data.
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
    std::shared_mutex m_MutexForSessions;
    std::shared_mutex m_MutexForValueUpdates;

  private: // command channel
    std::unique_ptr<ipc::TrOneShotServer<TrXRCommandMessage>> m_CommandChanServer = nullptr;
    std::unique_ptr<std::thread> m_CommandClientWatcher = nullptr;
    std::atomic<bool> m_CommandClientWatcherRunning = false;
    int m_AcceptTimeout = 1000;
  };
} // namespace xr
