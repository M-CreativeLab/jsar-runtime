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
  float *DeviceFrame::getViewerTransform() { return m_ViewerTransform; }
  glm::mat4 DeviceFrame::getLocalTransform(int id) { return m_XrDevice->getLocalTransformUnsafe(id); }

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
      int stereoId) : DeviceFrame(device)
  {
    m_IsMultiPass = true;
    m_CurrentStereoId = stereoId;
    m_ActiveEyeId = device->getActiveEyeId();

    auto viewerBaseMatrix = device->getViewerBaseMatrix();
    auto viewMatrix = device->getViewMatrixForEye(m_ActiveEyeId);
    auto projectionMatrix = device->getProjectionMatrixForEye(m_ActiveEyeId);
    memcpy(m_ViewerTransform, viewerBaseMatrix, sizeof(float) * 16);
    memcpy(m_ViewerViewMatrix, viewMatrix, sizeof(float) * 16);
    memcpy(m_ViewerProjectionMatrix, projectionMatrix, sizeof(float) * 16);
  }

  MultiPassFrame::~MultiPassFrame() {}

  int MultiPassFrame::getActiveEyeId() { return m_ActiveEyeId; }

  glm::mat4 MultiPassFrame::getViewMatrix(bool rightHanded)
  {
    if (rightHanded)
      return glm::make_mat4(m_ViewerViewMatrix);

    /**
     * When the matrix is expected to be left-handed, we get the base matrix for `viewMatrix` and convert it to left-handed.
     *
     * FIXME: Do we have more efficient way to convert the matrix to left-handed?
     */
    auto viewBaseMatrix = glm::inverse(glm::make_mat4(m_ViewerViewMatrix));
    return glm::inverse(math::convertBaseMatrixToLH(viewBaseMatrix));
  }

  glm::mat4 MultiPassFrame::getViewMatrixWithOffset(glm::mat4 &offsetTransform, bool rightHanded)
  {
    auto viewBaseMatrix = glm::inverse(glm::make_mat4(m_ViewerViewMatrix));
    glm::mat4 worldToLocal;
    if (rightHanded)
    {
      worldToLocal = glm::inverse(offsetTransform);
    }
    else
    {
      worldToLocal = glm::inverse(math::convertBaseMatrixToLH(offsetTransform));
      viewBaseMatrix = math::convertBaseMatrixToLH(viewBaseMatrix);
    }
    return glm::inverse(worldToLocal * viewBaseMatrix);
  }

  glm::mat4 MultiPassFrame::getProjectionMatrix(bool rightHanded)
  {
    auto projection = glm::make_mat4(m_ViewerProjectionMatrix);
    if (rightHanded)
      return projection;

    float *m = glm::value_ptr(projection);
    m[8] *= -1;
    m[9] *= -1;
    m[10] *= -1;
    m[11] *= -1;
    return projection;
  }

  glm::mat4 MultiPassFrame::computeMatrixByGraph(int contentId, commandbuffers::MatrixComputationGraph &computationGraph)
  {
    auto placeholder = computationGraph.placeholderId;
    auto isRightHandedSystem = computationGraph.handedness == commandbuffers::MatrixHandedness::MATRIX_RIGHT_HANDED;

    glm::mat4 matrix;
    /**
     * TODO: support real computation graph.
     */
    if (placeholder == WebGLMatrixPlaceholderId::ProjectionMatrix)
    {
      matrix = getProjectionMatrix(isRightHandedSystem);
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewMatrix)
    {
      auto contentLocal = getLocalTransform(contentId);
      auto originTransform = contentLocal * math::getOriginMatrix();
      matrix = getViewMatrixWithOffset(originTransform, isRightHandedSystem);
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewProjectionMatrix)
    {
      auto contentLocal = getLocalTransform(contentId);
      auto offsetTransform = contentLocal * math::getOriginMatrix();
      auto viewMatrix = getViewMatrixWithOffset(offsetTransform, isRightHandedSystem);
      matrix = getProjectionMatrix(isRightHandedSystem) * viewMatrix;
    }

    // Check if `inverseMatrix` is true.
    if (computationGraph.inverseMatrix)
      matrix = glm::inverse(matrix);
    return matrix;
  }

  static int s_NextStereoId = 1;
  StereoRenderingFrame::StereoRenderingFrame(bool isMultiPass)
  {
    m_IsMultiPass = isMultiPass;
    m_StereoId = s_NextStereoId++;
    m_CreatedTime = std::chrono::high_resolution_clock::now();
  }
  StereoRenderingFrame::~StereoRenderingFrame()
  {
    clearCommandBuffers();
  }

  FrameActionResult StereoRenderingFrame::startFrame(int passIndex)
  {
    if (passIndex > 1)
      return FRAME_PASS_OUT_OF_RANGE;

    m_Started[passIndex] = true;
    return FRAME_OK;
  }

  FrameActionResult StereoRenderingFrame::flushFrame(int passIndex)
  {
    if (passIndex > 1)
      return FRAME_PASS_OUT_OF_RANGE;

    m_ToFlush[passIndex] = true;
    m_Idempotentable = false; // NOTE: if a frame has flush command, it could not be idempotentable.
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
    copyCommandBuffers(frame->getCommandBuffers(0), 0);
    copyCommandBuffers(frame->getCommandBuffers(1), 1);
  }

  commandbuffers::TrCommandBufferBase *StereoRenderingFrame::cloneCommandBuffer(commandbuffers::TrCommandBufferBase *srcReq)
  {
    commandbuffers::TrCommandBufferBase *newReq = nullptr;
    switch (srcReq->type)
    {
#define XX(commandType, requestType)                              \
  case commandbuffers::COMMAND_BUFFER_##commandType##_REQ:        \
  {                                                               \
    newReq = dynamic_cast<commandbuffers::requestType *>(srcReq); \
    break;                                                        \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX
    default:
      DEBUG(LOG_TAG_ERROR, "Failed to clone an invalid command buffer(type=%d)", srcReq->type);
      break;
    }
    return newReq;
  }

  void StereoRenderingFrame::copyCommandBuffers(std::vector<commandbuffers::TrCommandBufferBase *> &commandBuffers, int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);

    clearCommandBuffers(passIndex);
    vector<commandbuffers::TrCommandBufferBase *> &targetList = passIndex == 0 ? m_CommandBuffersInPass : m_CommandBuffersInPass2;

    for (auto srcCommandBuffer : commandBuffers)
    {
      auto newReq = cloneCommandBuffer(srcCommandBuffer);
      if (newReq != nullptr)
      {
        targetList.push_back(newReq);
        m_CommandBuffersByteLength[passIndex] += newReq->size;
      }
    }
  }

  void StereoRenderingFrame::addCommandBuffer(commandbuffers::TrCommandBufferBase *commandBuffer, int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);

    auto &targetList = passIndex == 0 ? m_CommandBuffersInPass : m_CommandBuffersInPass2;
    targetList.push_back(commandBuffer);
    m_CommandBuffersByteLength[passIndex] += commandBuffer->size;

    /**
     * When the client is to add the following command buffers, this frame could be able to be dropped if necessary.
     */
    switch (commandBuffer->type)
    {
    // The command buffers which are initializing and creating resources would not be dropped.
    case commandbuffers::COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ:
    case commandbuffers::COMMAND_BUFFER_WEBGL2_CONTEXT_INIT_REQ:
    case commandbuffers::COMMAND_BUFFER_CREATE_PROGRAM_REQ:
    case commandbuffers::COMMAND_BUFFER_CREATE_SHADER_REQ:
    case commandbuffers::COMMAND_BUFFER_CHECK_FRAMEBUFFER_STATUS_REQ:
    case commandbuffers::COMMAND_BUFFER_ATTACH_SHADER_REQ:
    case commandbuffers::COMMAND_BUFFER_DETACH_SHADER_REQ:
    case commandbuffers::COMMAND_BUFFER_SHADER_SOURCE_REQ:
    case commandbuffers::COMMAND_BUFFER_COMPILE_SHADER_REQ:
    case commandbuffers::COMMAND_BUFFER_LINK_PROGRAM_REQ:
    // The command buffers which are getting values would not be dropped.
    case commandbuffers::COMMAND_BUFFER_GET_ERROR_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_BOOLEANV_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_FLOATV_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_INTEGERV_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_STRING_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_PROGRAM_PARAM_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_SAMPLER_PARAMETER_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_SHADER_INFO_LOG_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_SHADER_PARAM_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_SHADER_SOURCE_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_EXTENSIONS_REQ:
    case commandbuffers::COMMAND_BUFFER_GET_TRANSFORM_FEEDBACK_VARYING_REQ:
      m_Idempotentable = false;
      break;
    default:
      break;
    }
    m_IsAddedOnce = true;
  }

  std::vector<commandbuffers::TrCommandBufferBase *> &StereoRenderingFrame::getCommandBuffers(int passIndex)
  {
    if (passIndex == 0)
      return m_CommandBuffersInPass;
    else if (passIndex == 1)
      return m_CommandBuffersInPass2;
    else
      return m_CommandBuffersInPass;
  }

  bool StereoRenderingFrame::started()
  {
    if (m_IsMultiPass)
      return started(0) && started(1);
    else
      return started(0);
  }

  bool StereoRenderingFrame::started(int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);
    return m_Started[passIndex];
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

  bool StereoRenderingFrame::needFlush()
  {
    if (m_IsMultiPass)
      return needFlush(0) && needFlush(1);
    else
      return needFlush(0);
  }

  bool StereoRenderingFrame::needFlush(int passIndex)
  {
    if (passIndex > 1)
      return false;
    return m_ToFlush[passIndex];
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
  size_t StereoRenderingFrame::byteLength()
  {
    size_t len = byteLength(0);
    if (m_IsMultiPass)
      len += byteLength(1);
    return len;
  }
  size_t StereoRenderingFrame::byteLength(int passIndex)
  {
    return m_CommandBuffersByteLength[passIndex];
  }
  string StereoRenderingFrame::toString()
  {
    string frameDescription = "StereoFrame(" + std::to_string(m_StereoId) + "):";
    string isAvailableStr = m_Available ? "Yes" : "No";
    frameDescription += (" available(" + isAvailableStr + ")");
    frameDescription += (" started(" + std::to_string(m_Started[0]) + "," + std::to_string(m_Started[1]) + ")");
    frameDescription += (" ended(" + std::to_string(m_Ended[0]) + "," + std::to_string(m_Ended[1]) + ")");
    frameDescription += (" commands(" + std::to_string(m_CommandBuffersInPass.size()) + "," + std::to_string(m_CommandBuffersInPass2.size()) + ")");
    return frameDescription;
  }

  void StereoRenderingFrame::clearCommandBuffers()
  {
    m_CommandBuffersInPass.clear();
    m_CommandBuffersInPass2.clear();
  }

  void StereoRenderingFrame::clearCommandBuffers(int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);
    auto &targetList = passIndex == 0 ? m_CommandBuffersInPass : m_CommandBuffersInPass2;
    for (auto commandBuffer : targetList)
      delete commandBuffer;
    targetList.clear();
    m_CommandBuffersByteLength[passIndex] = 0;
  }
}
