#include <cstring>
#include "frame.hpp"

namespace xr
{
  FrameContextBySessionId::FrameContextBySessionId(int sessionId) : m_SessionId(sessionId)
  {
  }
  FrameContextBySessionId::~FrameContextBySessionId() {}

  int FrameContextBySessionId::getSessionId() { return m_SessionId; }
  float *FrameContextBySessionId::getLocalTransform() { return m_LocalTransform; }
  void FrameContextBySessionId::setLocalTransform(float *transform)
  {
    memcpy(m_LocalTransform, transform, sizeof(float) * 16);
  }

  Frame::Frame() {}
  Frame::~Frame() {}

  void Frame::startFrame() { m_Ended = false; }
  void Frame::endFrame() { m_Ended = true; }
  bool Frame::isFrameEnded() { return m_Ended; }
  float Frame::getTimestamp() { return m_Timestamp; }

  FrameContextBySessionId *Frame::addSession(int sessionId)
  {
    auto context = new FrameContextBySessionId(sessionId);
    m_Sessions[sessionId] = context;
    return context;
  }

  void Frame::iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback)
  {
    for (auto &item : m_Sessions)
      callback(item.first, item.second);
  }

  size_t Frame::getCountOfSessions()
  {
    return m_Sessions.size();
  }

  MultiPassFrame::MultiPassFrame(int eyeId, float *viewerViewMatrix, float *viewerProjectionMatrix, float timestamp)
  {
    m_ActiveEyeId = eyeId;
    m_Timestamp = timestamp;
    memcpy(m_ViewerViewMatrix, viewerViewMatrix, sizeof(float) * 16);
    memcpy(m_ViewerProjectionMatrix, viewerProjectionMatrix, sizeof(float) * 16);
  }

  MultiPassFrame::~MultiPassFrame() {}

  int MultiPassFrame::getActiveEyeId() { return m_ActiveEyeId; }
  float *MultiPassFrame::getViewerViewMatrix() { return m_ViewerViewMatrix; }
  float *MultiPassFrame::getViewerProjectionMatrix() { return m_ViewerProjectionMatrix; }
}
