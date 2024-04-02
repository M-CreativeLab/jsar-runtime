#include <cstring>
#include "device.hpp"
#include "frame.hpp"
#include "debug.hpp"

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

  DeviceFrame::DeviceFrame(xr::Device *device) : m_XrDevice(device) {}
  DeviceFrame::~DeviceFrame() {}

  void DeviceFrame::start() { m_Ended = false; }
  void DeviceFrame::end() { m_Ended = true; }
  bool DeviceFrame::ended() { return m_Ended; }
  bool DeviceFrame::isMultiPass() { return m_IsMultiPass; }
  float DeviceFrame::getTimestamp() { return m_Timestamp; }
  float *DeviceFrame::getViewerTransform() { return m_ViewerTransform; }
  float *DeviceFrame::getLocalTransform(int sessionId)
  {
    return m_XrDevice->getLocalTransformUnsafe(sessionId);
  }

  FrameContextBySessionId *DeviceFrame::addSession(int sessionId)
  {
    auto context = new FrameContextBySessionId(sessionId);
    m_Sessions[sessionId] = context;
    return context;
  }

  void DeviceFrame::iterateSessions(std::function<void(int, FrameContextBySessionId *)> callback)
  {
    for (auto &item : m_Sessions)
      callback(item.first, item.second);
  }

  size_t DeviceFrame::getCountOfSessions()
  {
    return m_Sessions.size();
  }

  int DeviceFrame::getCurrentStereoId()
  {
    return m_CurrentStereoId;
  }

  MultiPassFrame::MultiPassFrame(
      xr::Device *device,
      int eyeId,
      int stereoId) : DeviceFrame(device)
  {
    m_IsMultiPass = true;
    m_CurrentStereoId = stereoId;
    m_ActiveEyeId = eyeId;
    m_Timestamp = device->getTime();

    auto viewerTransform = device->getViewerTransform();
    auto viewMatrix = device->getViewerStereoViewMatrix(eyeId);
    auto projectionMatrix = device->getViewerStereoProjectionMatrix(eyeId);
    memcpy(m_ViewerTransform, viewerTransform, sizeof(float) * 16);
    memcpy(m_ViewerViewMatrix, viewMatrix, sizeof(float) * 16);
    memcpy(m_ViewerProjectionMatrix, projectionMatrix, sizeof(float) * 16);
  }

  MultiPassFrame::~MultiPassFrame() {}

  int MultiPassFrame::getActiveEyeId() { return m_ActiveEyeId; }
  float *MultiPassFrame::getViewerViewMatrix() { return m_ViewerViewMatrix; }
  float *MultiPassFrame::getViewerProjectionMatrix() { return m_ViewerProjectionMatrix; }

  static int s_NextStereoId = 1;

  StereoRenderingFrame::StereoRenderingFrame(bool isMultiPass)
  {
    m_IsMultiPass = isMultiPass;
    m_StereoId = s_NextStereoId++;
  }
  StereoRenderingFrame::~StereoRenderingFrame() {}

  FrameActionResult StereoRenderingFrame::startFrame(int passIndex)
  {
    if (passIndex > 1)
      return FRAME_PASS_OUT_OF_RANGE;

    m_Started[passIndex] = true;
    return FRAME_OK;
  }

  FrameActionResult StereoRenderingFrame::endFrame(int passIndex)
  {
    if (passIndex > 1)
      return FRAME_PASS_OUT_OF_RANGE;

    m_Ended[passIndex] = true;
    return FRAME_OK;
  }

  void StereoRenderingFrame::copyCommandBuffers(std::vector<renderer::CommandBuffer *> &commandBuffers, int passIndex)
  {
    if (passIndex == 0)
    {
      clearCommandBuffers(m_CommandBuffersInPass);
      for (auto commandBuffer : commandBuffers)
        m_CommandBuffersInPass.push_back(commandBuffer);
    }
    else if (passIndex == 1)
    {
      clearCommandBuffers(m_CommandBuffersInPass2);
      for (auto commandBuffer : commandBuffers)
        m_CommandBuffersInPass2.push_back(commandBuffer);
    }
  }

  void StereoRenderingFrame::addCommandBuffer(renderer::CommandBuffer *commandBuffer, int passIndex)
  {
    if (passIndex == 0)
      m_CommandBuffersInPass.push_back(commandBuffer);
    else if (passIndex == 1)
      m_CommandBuffersInPass2.push_back(commandBuffer);

    m_IsAddedOnce = true;
  }

  std::vector<renderer::CommandBuffer *> &StereoRenderingFrame::getCommandBuffers(int passIndex)
  {
    if (passIndex == 0)
      return m_CommandBuffersInPass;
    else if (passIndex == 1)
      return m_CommandBuffersInPass2;
    else
      return m_CommandBuffersInPass;
  }

  bool StereoRenderingFrame::ended()
  {
    if (m_IsMultiPass)
      return ended(0) && ended(1);
    else
      return ended(0);
  }

  bool StereoRenderingFrame::ended(int passIndex)
  {
    if (passIndex > 1)
      return false;
    return m_Ended[passIndex];
  }

  int StereoRenderingFrame::getId() { return m_StereoId; }
  bool StereoRenderingFrame::addedOnce() { return m_IsAddedOnce; }

  void StereoRenderingFrame::clearCommandBuffers(std::vector<renderer::CommandBuffer *> &commandBuffers)
  {
    for (auto commandBuffer : commandBuffers)
      delete commandBuffer;
    commandBuffers.clear();
  }
}
