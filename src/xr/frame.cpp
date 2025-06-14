#include <cstring>
#include "device.hpp"
#include "frame.hpp"
#include "debug.hpp"

namespace xr
{
  FrameContextBySessionId::FrameContextBySessionId(int sessionId)
      : m_SessionId(sessionId)
  {
  }
  FrameContextBySessionId::~FrameContextBySessionId()
  {
  }

  int FrameContextBySessionId::getSessionId()
  {
    return m_SessionId;
  }
  float *FrameContextBySessionId::getLocalTransform()
  {
    return m_LocalTransform;
  }
  void FrameContextBySessionId::setLocalTransform(float *transform)
  {
    memcpy(m_LocalTransform, transform, sizeof(float) * 16);
  }

  void DeviceFrame::start()
  {
    m_Ended = false;
  }
  void DeviceFrame::end()
  {
    m_Ended = true;
  }
  bool DeviceFrame::ended()
  {
    return m_Ended;
  }
  bool DeviceFrame::isMultiPass()
  {
    return m_IsMultiPass;
  }
  float *DeviceFrame::getViewerTransform()
  {
    return m_ViewerTransform;
  }
  glm::mat4 DeviceFrame::getLocalTransform(int id)
  {
    return m_XrDevice->getLocalTransformUnsafe(id);
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

  MultiPassFrame::MultiPassFrame(
    xr::Device *device,
    int stereoId)
      : DeviceFrame(device)
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

  MultiPassFrame::~MultiPassFrame()
  {
  }

  int MultiPassFrame::getActiveEyeId()
  {
    return m_ActiveEyeId;
  }

  /**
   * Get glm::mat4 from a view matrix float array.
   */
  inline glm::mat4 GetViewMat(float *sourceViewMatrixFloats, bool rightHanded)
  {
    if (rightHanded)
      return glm::make_mat4(sourceViewMatrixFloats);

    /**
     * When the matrix is expected to be left-handed, we get the base matrix for `viewMatrix` and convert it to left-handed.
     *
     * FIXME: Do we have more efficient way to convert the matrix to left-handed?
     */
    auto viewBaseMatrix = glm::inverse(glm::make_mat4(sourceViewMatrixFloats));
    return glm::inverse(math::ConvertBaseMatrixToLH(viewBaseMatrix));
  }

  /**
   * Get glm::mat4 from a view matrix float array with an offset transform.
   */
  inline glm::mat4 GetViewMatWithOffset(float *sourceViewMatrixFloats, glm::mat4 &offsetTransform, bool rightHanded)
  {
    auto viewBaseMatrix = glm::inverse(glm::make_mat4(sourceViewMatrixFloats));
    glm::mat4 worldToLocal;
    if (rightHanded)
    {
      worldToLocal = glm::inverse(offsetTransform);
    }
    else
    {
      worldToLocal = glm::inverse(math::ConvertBaseMatrixToLH(offsetTransform));
      viewBaseMatrix = math::ConvertBaseMatrixToLH(viewBaseMatrix);
    }
    return glm::inverse(worldToLocal * viewBaseMatrix);
  }

  /**
   * Get glm::mat4 from a projection matrix float array.
   */
  inline glm::mat4 GetProjectionMat(float *sourceProjectionMatrixFloats, bool rightHanded)
  {
    auto projection = glm::make_mat4(sourceProjectionMatrixFloats);
    if (rightHanded)
      return projection;

    float *m = glm::value_ptr(projection);
    m[8] *= -1;
    m[9] *= -1;
    m[10] *= -1;
    m[11] *= -1;
    return projection;
  }

  glm::mat4 MultiPassFrame::getViewMatrix(bool rightHanded)
  {
    return GetViewMat(m_ViewerViewMatrix, rightHanded);
  }

  glm::mat4 MultiPassFrame::getViewMatrixWithOffset(glm::mat4 &offsetTransform, bool rightHanded)
  {
    return GetViewMatWithOffset(m_ViewerViewMatrix, offsetTransform, rightHanded);
  }

  glm::mat4 MultiPassFrame::getProjectionMatrix(bool rightHanded)
  {
    return GetProjectionMat(m_ViewerProjectionMatrix, rightHanded);
  }

  glm::mat4 MultiPassFrame::computeMatrixByGraph(commandbuffers::MatrixComputationGraph &computationGraph, int sessionId, int viewIndex)
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
      auto contentLocal = getLocalTransform(sessionId);
      auto originTransform = contentLocal * math::GetOriginMatrix();
      matrix = getViewMatrixWithOffset(originTransform, isRightHandedSystem);
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewProjectionMatrix)
    {
      auto contentLocal = getLocalTransform(sessionId);
      auto offsetTransform = contentLocal * math::GetOriginMatrix();
      auto viewMatrix = getViewMatrixWithOffset(offsetTransform, isRightHandedSystem);
      matrix = getProjectionMatrix(isRightHandedSystem) * viewMatrix;
    }

    // Check if `inverseMatrix` is true.
    if (computationGraph.inverseMatrix)
      matrix = glm::inverse(matrix);
    return matrix;
  }

  SinglePassFrame::SinglePassFrame(xr::Device *device, int stereoId)
      : DeviceFrame(device)
  {
    m_IsMultiPass = false;
    m_CurrentStereoId = stereoId;

    auto viewerBaseMatrix = device->getViewerBaseMatrix();
    memcpy(m_ViewerTransform, viewerBaseMatrix, sizeof(float) * 16);

    {
      // Setup for left eye
      auto viewMatrix = device->getViewMatrixForEye(0);
      auto projectionMatrix = device->getProjectionMatrixForEye(0);
      memcpy(m_ViewMatrixForLeftEye, viewMatrix, sizeof(float) * 16);
      memcpy(m_ProjectionMatrixForLeftEye, projectionMatrix, sizeof(float) * 16);
    }
    {
      // Setup for right eye
      auto viewMatrix = device->getViewMatrixForEye(1);
      auto projectionMatrix = device->getProjectionMatrixForEye(1);
      memcpy(m_ViewMatrixForRightEye, viewMatrix, sizeof(float) * 16);
      memcpy(m_ProjectionMatrixForRightEye, projectionMatrix, sizeof(float) * 16);
    }
  }

  glm::mat4 SinglePassFrame::computeMatrixByGraph(commandbuffers::MatrixComputationGraph &computationGraph, int sessionId, int viewIndex)
  {
    auto placeholder = computationGraph.placeholderId;
    auto isRightHandedSystem = computationGraph.handedness == commandbuffers::MatrixHandedness::MATRIX_RIGHT_HANDED;

    auto projectionMatrixFloats = viewIndex == 0 ? m_ProjectionMatrixForLeftEye : m_ProjectionMatrixForRightEye;
    auto viewMatrixFloats = viewIndex == 0 ? m_ViewMatrixForLeftEye : m_ViewMatrixForRightEye;

    glm::mat4 matrix;
    /**
     * TODO: support real computation graph.
     */
    if (placeholder == WebGLMatrixPlaceholderId::ProjectionMatrix)
    {
      matrix = GetProjectionMat(projectionMatrixFloats, isRightHandedSystem);
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewMatrix)
    {
      auto contentLocal = getLocalTransform(sessionId);
      auto originTransform = contentLocal * math::GetOriginMatrix();
      matrix = GetViewMatWithOffset(viewMatrixFloats, originTransform, isRightHandedSystem);
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewProjectionMatrix)
    {
      auto contentLocal = getLocalTransform(sessionId);
      auto offsetTransform = contentLocal * math::GetOriginMatrix();
      auto viewMatrix = GetViewMatWithOffset(viewMatrixFloats, offsetTransform, isRightHandedSystem);
      matrix = GetProjectionMat(projectionMatrixFloats, isRightHandedSystem) * viewMatrix;
    }
    else if (placeholder == WebGLMatrixPlaceholderId::ViewProjectionMatrixForRightEye)
    {
      auto contentLocal = getLocalTransform(sessionId);
      auto offsetTransform = contentLocal * math::GetOriginMatrix();
      auto viewMatrix = GetViewMatWithOffset(m_ViewMatrixForRightEye, offsetTransform, isRightHandedSystem);
      matrix = GetProjectionMat(m_ProjectionMatrixForRightEye, isRightHandedSystem) * viewMatrix;
    }

    // Check if `inverseMatrix` is true.
    if (computationGraph.inverseMatrix)
      matrix = glm::inverse(matrix);
    return matrix;
  }

  StereoRenderingFrame::StereoRenderingFrame(bool isMultiPass, int id)
  {
    m_IsMultiPass = isMultiPass;
    m_StereoId = id;
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

  void StereoRenderingFrame::moveCommandBuffersTo(StereoRenderingFrame &dst, int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);
    dst.clearCommandBuffers(passIndex); // Clear the indexed command buffers before moving new command buffers.

    auto &srcList = passIndex == 0 ? m_CommandBuffersInPass : m_CommandBuffersInPass2;
    auto &dstList = passIndex == 0 ? dst.m_CommandBuffersInPass : dst.m_CommandBuffersInPass2;

    for (auto req : srcList)
    {
      if (TR_LIKELY(req != nullptr))
      {
        dstList.push_back(req);
        dst.m_CommandBuffersByteLength[passIndex] += req->size;
      }
    }
    srcList.clear();
    m_CommandBuffersByteLength[passIndex] = 0;
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
    return m_IsMultiPass ? m_Started[passIndex] : m_Started[0];
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
    if (passIndex > 1 || passIndex < 0)
      return false;
    return m_IsMultiPass ? m_Ended[passIndex] : m_Ended[0];
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
    return m_IsMultiPass ? m_ToFlush[passIndex] : m_ToFlush[0];
  }

  void StereoRenderingFrame::resetFlush(int passIndex)
  {
    if (passIndex > 1)
      return;
    m_ToFlush[passIndex] = false;
  }

  bool StereoRenderingFrame::idempotent()
  {
    if (m_Idempotentable == false)
      return false;
    if (m_IsMultiPass)
      return m_Idempotent[0] && m_Idempotent[1];
    else
      return m_Idempotent[0];
  }

  void StereoRenderingFrame::idempotent(int passIndex, bool value)
  {
    if (passIndex > 1 || passIndex < 0)
      return;
    m_Idempotent[passIndex] = value;
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

  void StereoRenderingFrame::clearCommandBuffers(int passIndex)
  {
    assert(passIndex == 0 || passIndex == 1);
    auto &targetList = passIndex == 0 ? m_CommandBuffersInPass : m_CommandBuffersInPass2;
    for (auto commandBuffer : targetList)
    {
      if (commandBuffer != nullptr)
        delete commandBuffer;
    }
    targetList.clear();
    m_CommandBuffersByteLength[passIndex] = 0;
  }
}
