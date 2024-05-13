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

  DeviceFrame::DeviceFrame(xr::Device *device) : m_XrDevice(device)
  {
    // Copy the input sources for this frame from the device.
    auto gazeInputSource = device->getGazeInputSource();
    m_GazeInputSource = gazeInputSource != nullptr ? InputSource(gazeInputSource) : InputSource();

    auto leftHandInputSource = device->getHandInputSource(Handness::Left);
    m_HandInputSources.push_back(leftHandInputSource != nullptr ? InputSource(leftHandInputSource) : InputSource());

    auto rightHandInputSource = device->getHandInputSource(Handness::Right);
    m_HandInputSources.push_back(rightHandInputSource != nullptr ? InputSource(rightHandInputSource) : InputSource());
  }
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

  FrameContextBySessionId *DeviceFrame::getSession(int sessionId)
  {
    auto it = m_Sessions.find(sessionId);
    if (it != m_Sessions.end())
      return it->second;
    return nullptr;
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

  void DeviceFrame::setStereoId(int id)
  {
    m_CurrentStereoId = id;
  }

  InputSource &DeviceFrame::getGazeInputSource()
  {
    return m_GazeInputSource;
  }

  InputSource &DeviceFrame::getHandInputSource(Handness handness)
  {
    if (handness == Handness::Left)
      return m_HandInputSources[0];
    else
      return m_HandInputSources[1];
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
    m_CreatedTime = std::chrono::high_resolution_clock::now();
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

  void StereoRenderingFrame::copyCommandBuffers(StereoRenderingFrame *frame)
  {
    copyCommandBuffers(frame->m_CommandBuffersInPass, 0);
    copyCommandBuffers(frame->m_CommandBuffersInPass2, 1);
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

    /**
     * When the client is to add the following command buffers, this frame could be able to be dropped if necessary.
     */
    switch (commandBuffer->GetType())
    {
    // The command buffers which are initializing and creating resources would not be dropped.
    case renderer::kCommandTypeContextInit:
    case renderer::kCommandTypeContext2Init:
    case renderer::kCommandTypeCreateProgram:
    case renderer::kCommandTypeCreateShader:
    case renderer::kCommandTypeCheckFramebufferStatus:
    case renderer::kCommandTypeAttachShader:
    case renderer::kCommandTypeDetachShader:
    case renderer::kCommandTypeShaderSource:
    case renderer::kCommandTypeCompileShader:
    case renderer::kCommandTypeLinkProgram:
    // The command buffers which are getting values would not be dropped.
    case renderer::kCommandTypeGetAttribLocation:
    case renderer::kCommandTypeGetBooleanv:
    case renderer::kCommandTypeGetError:
    case renderer::kCommandTypeGetFloatv:
    case renderer::kCommandTypeGetIntegerv:
    case renderer::kCommandTypeGetProgramInfoLog:
    case renderer::kCommandTypeGetProgramParameter:
    case renderer::kCommandTypeGetSamplerParameter:
    case renderer::kCommandTypeGetShaderInfoLog:
    case renderer::kCommandTypeGetShaderParameter:
    case renderer::kCommandTypeGetShaderPrecisionFormat:
    case renderer::kCommandTypeGetShaderSource:
    case renderer::kCommandTypeGetString:
    case renderer::kCommandTypeGetSupportedExtensions:
    case renderer::kCommandTypeGetTransformFeedbackVarying:
    case renderer::kCommandTypeGetUniformLocation:
      m_Idempotentable = false;
      break;
    default:
      break;
    }
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
  bool StereoRenderingFrame::empty()
  {
    if (m_IsMultiPass)
      return m_CommandBuffersInPass.empty() && m_CommandBuffersInPass2.empty();
    else
      return m_CommandBuffersInPass.empty();
  }
  bool StereoRenderingFrame::available()
  {
    return m_Available;
  }
  void StereoRenderingFrame::available(bool v)
  {
    m_Available = v;
  }
  bool StereoRenderingFrame::expired(int timeout)
  {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_CreatedTime);
    return duration.count() > timeout;
  }
  bool StereoRenderingFrame::idempotent()
  {
    if (m_Idempotentable == false)
      return false;
    return m_Idempotent[0] && m_Idempotent[1];
  }
  void StereoRenderingFrame::idempotent(int passIndex, bool value)
  {
    if (passIndex > 1 || passIndex < 0)
      return;
    m_Idempotent[passIndex] = value;
  }
  void StereoRenderingFrame::finishPass(int passIndex)
  {
    if (passIndex > 1 || passIndex < 0)
      return;
    m_Finished[passIndex] = true;
  }
  bool StereoRenderingFrame::finished(int passIndex)
  {
    if (passIndex > 1 || passIndex < 0)
      return false;
    return m_Finished[passIndex];
  }

  void StereoRenderingFrame::clearCommandBuffers()
  {
    clearCommandBuffers(m_CommandBuffersInPass);
    clearCommandBuffers(m_CommandBuffersInPass2);
  }

  void StereoRenderingFrame::clearCommandBuffers(std::vector<renderer::CommandBuffer *> &commandBuffers)
  {
    for (auto commandBuffer : commandBuffers)
      delete commandBuffer;
    commandBuffers.clear();
  }
}
