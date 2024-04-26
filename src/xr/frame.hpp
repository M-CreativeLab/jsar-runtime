#pragma once

#include <map>
#include <vector>
#include "renderer/command_buffer.hpp"

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
    float getTimestamp();
    float *getViewerTransform();
    float *getLocalTransform(int sessionId);
    FrameContextBySessionId *addSession(int sessionId);
    FrameContextBySessionId *getSession(int sessionId);
    void iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback);

    size_t getCountOfSessions();
    int getCurrentStereoId();

  protected:
    Device *m_XrDevice = nullptr;
    bool m_Ended = false;
    bool m_IsMultiPass = false;
    float m_Timestamp = 0;
    float m_ViewerTransform[16];
    std::map<int, FrameContextBySessionId *> m_Sessions;
    int m_CurrentStereoId = -1;
  };

  class MultiPassFrame : public DeviceFrame
  {
  public:
    explicit MultiPassFrame(Device *device, int eyeId, int stereoId);
    ~MultiPassFrame();

  public:
    int getActiveEyeId();
    float *getViewerViewMatrix();
    float *getViewerProjectionMatrix();

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

  class StereoRenderingFrame
  {
  public:
    StereoRenderingFrame(bool isMultiPass);
    ~StereoRenderingFrame();

  public:
    FrameActionResult startFrame(int passIndex = 0);
    FrameActionResult endFrame(int passIndex = 0);
    void copyCommandBuffers(std::vector<renderer::CommandBuffer *> &commandBuffers, int passIndex = 0);
    void addCommandBuffer(renderer::CommandBuffer *commandBuffer, int passIndex = 0);
    std::vector<renderer::CommandBuffer *> &getCommandBuffers(int passIndex = 0);
    bool ended();
    bool ended(int passIndex);
    int getId();
    bool addedOnce();
    bool empty();
    void finishPass(int passIndex);
    bool finished(int passIndex);

  private:
    void clearCommandBuffers(std::vector<renderer::CommandBuffer *> &commandBuffers);

  private:
    int m_StereoId = -1;
    bool m_IsMultiPass = true;
    bool m_Ended[2] = {false, false};
    bool m_Started[2] = {false, false};
    bool m_Finished[2] = {false, false};
    bool m_IsAddedOnce = false;
    std::vector<renderer::CommandBuffer *> m_CommandBuffersInPass;
    std::vector<renderer::CommandBuffer *> m_CommandBuffersInPass2; // This is only used when m_IsMultiPass is true.
    // TODO: support 3rd, 4th, ... passes?
  };
} // namespace xr
