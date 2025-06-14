#pragma once

#include <map>
#include <vector>
#include <chrono>
#include "math/matrix.hpp"
#include "input_source.hpp"
// #include "renderer/command_buffer.hpp"
#include "common/command_buffers/shared.hpp"
#include "common/command_buffers/macros.hpp"
#include "common/command_buffers/command_buffers.hpp"

using namespace std;

namespace xr
{
  class Device;

  /**
   * This is used to store the local transform matrix for a specific WebXR session.
   */
  class FrameContextBySessionId
  {
  public:
    /**
     * Constructor.
     *
     * @param sessionId The session id.
     */
    FrameContextBySessionId(int sessionId);
    ~FrameContextBySessionId();

  public:
    /**
     * @returns The session id.
     */
    int getSessionId();
    /**
     * @returns The local transform matrix.
     */
    float *getLocalTransform();
    /**
     * Set the local transform matrix.
     */
    void setLocalTransform(float *transform);

  private:
    int m_SessionId = -1;
    float m_LocalTransform[16];
  };

  /**
   * The abstract class to represent a device-specific frame. The `DeviceFrame` manages the frame context for each session,
   * and provides its stereo-related information including:
   *
   * - Rendering mode (single-pass or multi-pass)
   * - Viewer transform
   * - Local transform for each session
   * - Input sources
   */
  class DeviceFrame
  {
  public:
    /**
     * Construct a `DeviceFrame` with a specific XR device.
     *
     * @param device The XR device.
     */
    DeviceFrame(xr::Device *device)
        : m_XrDevice(device)
    {
    }
    virtual ~DeviceFrame() = default;

  public:
    /**
     * Mark the device frame as started.
     */
    void start();
    /**
     * Mark the device frame as ended.
     */
    void end();
    /**
     * @returns If the device frame is ended.
     */
    bool ended();
    /**
     * @returns If the device frame is multi-pass.
     */
    bool isMultiPass();
    /**
     * @returns The viewer transform matrix in float array.
     */
    float *getViewerTransform();
    /**
     * @param sessionId The session id.
     * @returns The local transform matrix for a specific session.
     */
    glm::mat4 getLocalTransform(int sessionId);
    /**
     * Add a session to the device frame's sessions.
     *
     * @param sessionId The session id.
     * @returns The `FrameContextBySessionId` for the session.
     */
    FrameContextBySessionId *addSession(int sessionId);
    /**
     * @param sessionId The session id.
     * @returns The `FrameContextBySessionId` for a specific session.
     */
    FrameContextBySessionId *getSession(int sessionId);
    /**
     * Iterate all sessions in the device frame.
     *
     * @param callback The callback function to be called for each session.
     */
    void iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback);

    /**
     * @returns The count of sessions in the device frame.
     */
    size_t getCountOfSessions();
    /**
     * @returns The current stereo id.
     */
    int getCurrentStereoId();
    /**
     * Set the current stereo id.
     */
    void setStereoId(int id);

    /**
     * @returns The gaze input source.
     */
    InputSource &getGazeInputSource();
    /**
     * @param handness The handness.
     * @returns The hand input source.
     */
    InputSource &getHandInputSource(Handness handness);

  public:
    /**
     * It computes the matrix by the given computation graph, every device frame implementation should implement this method.
     *
     * @param computationGraph The computation graph.
     * @param sessionId The session id.
     * @param viewIndex The view index.
     * @returns The computed matrix by the input computation graph.
     */
    virtual glm::mat4 computeMatrixByGraph(commandbuffers::MatrixComputationGraph &computationGraph, int sessionId, int viewIndex) = 0;

  protected:
    Device *m_XrDevice = nullptr;
    bool m_Ended = false;
    bool m_IsMultiPass = false;
    float m_ViewerTransform[16];
    std::map<int, FrameContextBySessionId *> m_Sessions;
    int m_CurrentStereoId = -1;
  };

  /**
   * The `DeviceFrame` for multi-pass rendering.
   */
  class MultiPassFrame : public DeviceFrame
  {
  public:
    /**
     * Construct a `MultiPassFrame` with a specific XR device and stereo id.
     *
     * @param device The XR device.
     * @param stereoId The stereo id.
     */
    explicit MultiPassFrame(Device *device, int stereoId);
    ~MultiPassFrame();

  public:
    /**
     * @returns The active eye id.
     */
    int getActiveEyeId();
    /**
     * It returns the view matrix for the active eye.
     *
     * @param rightHanded If true, the view matrix is right-handed, otherwise left-handed.
     */
    glm::mat4 getViewMatrix(bool rightHanded = true);
    /**
     * It returns the view matrix relative to the offset transform.
     *
     * @param offsetTransform The offset transform matrix.
     * @param rightHanded If true, the view matrix is right-handed, otherwise left-handed.
     */
    glm::mat4 getViewMatrixWithOffset(glm::mat4 &offsetTransform, bool rightHanded = true);
    /**
     * It returns the projection matrix for the active eye.
     *
     * @param rightHanded If true, the projection matrix is right-handed, otherwise left-handed.
     */
    glm::mat4 getProjectionMatrix(bool rightHanded = true);
    /**
     * It does operate the computation graph based on current frame, and returns glm::mat4.
     *
     * @param computationGraph the computation graph to be operated.
     * @param sessionId the session id.
     * @param viewIndex the view index.
     */
    glm::mat4 computeMatrixByGraph(commandbuffers::MatrixComputationGraph &computationGraph, int sessionId, int viewIndex) override;

  private:
    int m_ActiveEyeId = -1;
    float m_ViewerViewMatrix[16];       // The viewer's view matrix for the active eye in current pass.
    float m_ViewerProjectionMatrix[16]; // The viewer's projection matrix for the active eye in current pass.
  };

  /**
   * The `DeviceFrame` for single-pass rendering.
   */
  class SinglePassFrame : public DeviceFrame
  {
  public:
    /**
     * Construct a `SinglePassFrame` with a specific XR device and stereo id.
     *
     * @param device The XR device.
     * @param stereoId The stereo id.
     */
    explicit SinglePassFrame(Device *device, int stereoId);

  public:
    glm::mat4 computeMatrixByGraph(commandbuffers::MatrixComputationGraph &computationGraph, int sessionId, int viewIndex) override;

  private:
    float m_ViewMatrixForLeftEye[16];
    float m_ProjectionMatrixForLeftEye[16];
    float m_ViewMatrixForRightEye[16];
    float m_ProjectionMatrixForRightEye[16];
  };

  enum FrameActionResult
  {
    FRAME_OK = 0x00,
    FRAME_PASS_OUT_OF_RANGE,
  };

  /**
   * This class represents a stereo rendering frame, which stores the command buffers at JS thread, and execute them at
   * rendering thread.
   */
  class StereoRenderingFrame
  {
  public:
    StereoRenderingFrame(bool isMultiPass, int id);
    ~StereoRenderingFrame();

  public:
    FrameActionResult startFrame(int passIndex = 0);
    FrameActionResult flushFrame(int passIndex = 0);
    FrameActionResult endFrame(int passIndex = 0);
    commandbuffers::TrCommandBufferBase *cloneCommandBuffer(commandbuffers::TrCommandBufferBase *src);
    inline void moveCommandBuffersTo(StereoRenderingFrame &dst)
    {
      moveCommandBuffersTo(dst, 0);
      moveCommandBuffersTo(dst, 1);
    }
    void moveCommandBuffersTo(StereoRenderingFrame &dst, int passIndex);
    void addCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer, int passIndex = 0);
    vector<commandbuffers::TrCommandBufferBase *> &getCommandBuffers(int passIndex = 0);
    bool started();
    bool started(int passIndex);
    bool ended();
    bool ended(int passIndex);
    bool needFlush();
    bool needFlush(int passIndex);
    /**
     * In a single frame, we may have multiple flush commands, this method is used to reset the flush flag.
     */
    void resetFlush(int passIndex);
    inline int getId()
    {
      return m_StereoId;
    }
    inline bool addedOnce()
    {
      return m_IsAddedOnce;
    }
    inline bool empty()
    {
      if (m_IsMultiPass)
        return m_CommandBuffersInPass.empty() && m_CommandBuffersInPass2.empty();
      else
        return m_CommandBuffersInPass.empty();
    }
    inline bool available()
    {
      return m_Available;
    }
    inline void available(bool v)
    {
      m_Available = v;
    }
    // returns if this frame is expired after `timeout` milliseconds.
    inline bool expired(int timeout)
    {
      return duration<chrono::milliseconds>().count() > timeout;
    }
    bool idempotent();
    void idempotent(int passIndex, bool value);
    inline void finishPass(int passIndex)
    {
      assert(passIndex == 0 || passIndex == 1);
      m_Finished[passIndex] = true;
    }
    inline bool finished(int passIndex)
    {
      return passIndex < 0 || passIndex > 1 ? false : m_Finished[passIndex];
    }

    inline size_t count()
    {
      return m_CommandBuffersInPass.size() + m_CommandBuffersInPass2.size();
    }
    inline size_t count(int passIndex)
    {
      return passIndex == 0 ? m_CommandBuffersInPass.size() : m_CommandBuffersInPass2.size();
    }
    inline size_t byteLength()
    {
      size_t len = byteLength(0);
      if (m_IsMultiPass)
        len += byteLength(1);
      return len;
    }
    inline size_t byteLength(int passIndex)
    {
      return m_CommandBuffersByteLength[passIndex];
    }
    string toString();

  public:
    template <typename T>
    inline T duration()
    {
      auto now = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<T>(now - m_CreatedTime);
    }

  private:
    inline void clearCommandBuffers()
    {
      clearCommandBuffers(0);
      clearCommandBuffers(1);
    }
    void clearCommandBuffers(int passIndex);

  private:
    int m_StereoId = -1;
    bool m_IsMultiPass = true;
    bool m_Available = false;
    bool m_Ended[2] = {false, false};
    bool m_Started[2] = {false, false};
    bool m_ToFlush[2] = {false, false};
    /**
     * An idempotent frame is free to replay.
     */
    bool m_Idempotent[2] = {false, false};
    /**
     * The *Idempotentable* flag is a hint to the renderer that the frame is able to be idempotent. Such as the frame that contains
     * a `linkProgram()`, `createProgram()` or other specific command buffers, it could not be idempotent.
     */
    bool m_Idempotentable = true;
    bool m_Finished[2] = {false, false};
    bool m_IsAddedOnce = false;

    /**
     * Record the following time points for frame drops and performance analysis.
     */
    chrono::time_point<chrono::high_resolution_clock> m_CreatedTime;
    chrono::time_point<chrono::high_resolution_clock> m_EndedTime;

    size_t m_CommandBuffersByteLength[2] = {0, 0};
    vector<commandbuffers::TrCommandBufferBase *> m_CommandBuffersInPass;
    vector<commandbuffers::TrCommandBufferBase *> m_CommandBuffersInPass2; // This is only used when m_IsMultiPass is true.
    // TODO: support 3rd, 4th, ... passes?

    friend class Device;
    friend class renderer::TrContentRenderer;
  };
} // namespace xr
