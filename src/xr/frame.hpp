#pragma once

#include <map>

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

  class Frame
  {
  public:
    Frame();
    ~Frame();

  public:
    void startFrame();
    void endFrame();
    bool isFrameEnded();
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

  class MultiPassFrame : public Frame
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
} // namespace xr
