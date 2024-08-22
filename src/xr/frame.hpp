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

  class FrameContextBySessionId
  {
  public:
    FrameContextBySessionId(int sessionId);
    ~FrameContextBySessionId();

  public:
    int getSessionId();
    float *getLocalTransform();
    void setLocalTransform(float *transform);

  private:
    int m_SessionId = -1;
    float m_LocalTransform[16];
  };

  class DeviceFrame
  {
  public:
    DeviceFrame(xr::Device *device);
    ~DeviceFrame();

  public:
    void start();
    void end();
    bool ended();
    bool isMultiPass();
    float *getViewerTransform();
    glm::mat4 getLocalTransform(int sessionId);
    FrameContextBySessionId *addSession(int sessionId);
    FrameContextBySessionId *getSession(int sessionId);
    void iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback);

    size_t getCountOfSessions();
    int getCurrentStereoId();
    void setStereoId(int id);

    InputSource &getGazeInputSource();
    InputSource &getHandInputSource(Handness handness);

  protected:
    Device *m_XrDevice = nullptr;
    bool m_Ended = false;
    bool m_IsMultiPass = false;
    float m_ViewerTransform[16];
    std::map<int, FrameContextBySessionId *> m_Sessions;
    int m_CurrentStereoId = -1;
  };

  class MultiPassFrame : public DeviceFrame
  {
  public:
    explicit MultiPassFrame(Device *device, int stereoId);
    ~MultiPassFrame();

  public:
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
     */
    glm::mat4 computeMatrixByGraph(int sessionId, commandbuffers::MatrixComputationGraph &computationGraph);

  private:
    int m_ActiveEyeId = -1;
    /**
     * The viewer's view matrix for the active eye in current pass.
     */
    float m_ViewerViewMatrix[16];
    /**
     * The viewer's projection matrix for the active eye in current pass.
     */
    float m_ViewerProjectionMatrix[16];
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
    void copyCommandBuffers(StereoRenderingFrame *frame);
    void copyCommandBuffers(vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, int passIndex = 0);
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
    int getId();
    bool addedOnce();
    bool empty();
    bool available();
    void available(bool v);
    bool expired(int timeout); // returns if this frame is expired after `timeout` milliseconds.
    bool idempotent();
    void idempotent(int passIndex, bool value);
    void finishPass(int passIndex);
    bool finished(int passIndex);
    size_t byteLength();
    size_t byteLength(int passIndex);
    string toString();

  public:
    template <typename T>
    inline T duration()
    {
      auto now = std::chrono::high_resolution_clock::now();
      return std::chrono::duration_cast<T>(now - m_CreatedTime);
    }

  private:
    void clearCommandBuffers();
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
