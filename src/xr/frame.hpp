#pragma once

#include <map>
#include <vector>
#include "renderer/command_buffer.hpp"

namespace xr
{
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
    DeviceFrame();
    ~DeviceFrame();

  public:
    void start();
    void end();
    bool ended();
    float getTimestamp();
    float *getViewerTransform();
    FrameContextBySessionId *addSession(int sessionId);
    void iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback);
    size_t getCountOfSessions();

  protected:
    bool m_Ended = false;
    float m_Timestamp = 0;
    float m_ViewerTransform[16];
    std::map<int, FrameContextBySessionId *> m_Sessions;
  };

  class MultiPassFrame : public DeviceFrame
  {
  public:
    explicit MultiPassFrame(
        int eyeId,
        float *viewerTransform,
        float *viewerViewMatrix,
        float *viewerProjectionMatrix,
        float timestamp);
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

  class StereoRenderingFrame
  {
  public:
    StereoRenderingFrame(bool isMultiPass);
    ~StereoRenderingFrame();

  public:
    void startFrame(int passIndex = 0);
    void endFrame(int passIndex = 0);
    void addCommandBuffer(renderer::CommandBuffer *commandBuffer, int passIndex = 0);
    bool ended();
    bool ended(int passIndex);

  private:
    int m_StereoId = -1;
    bool m_IsMultiPass = true;
    bool m_Ended[2] = {false, false};
    bool m_Started[2] = {false, false};
    std::vector<renderer::CommandBuffer *> m_CommandBuffersInPass;
    std::vector<renderer::CommandBuffer *> m_CommandBuffersInPass2; // This is only used when m_IsMultiPass is true.
    // TODO: support 3rd, 4th, ... passes?
  };
} // namespace xr
