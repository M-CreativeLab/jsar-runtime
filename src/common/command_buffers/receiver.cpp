#include "./receiver.hpp"
#include "./command_buffers.hpp"
#include "./macros.hpp"

namespace commandbuffers
{
  TrCommandBufferBase *TrCommandBufferReceiver::recvCommandBufferRequest(int timeout)
  {
    TrCommandBufferMessage message;
    if (!message.deserialize(this, timeout))
      return nullptr;

    TrCommandBufferBase *req = nullptr;
    switch (message.type)
    {
#define XX(commandType, requestType)                                    \
  case COMMAND_BUFFER_##commandType##_REQ:                              \
  {                                                                     \
    req = TrCommandBufferBase::CreateFromMessage<requestType>(message); \
    break;                                                              \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX
    default:
      DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
      break;
    }
    return req;
  }

  TrCommandBufferResponse *TrCommandBufferReceiver::recvCommandBufferResponse(int timeout)
  {
    TrCommandBufferMessage message;
    if (!message.deserialize(this, timeout))
      return nullptr;

    TrCommandBufferResponse *resp = nullptr;
    switch (message.type)
    {
#define CASE_COMMAND_BUFFER_RESPONSE(commandType, responseType)           \
  case COMMAND_BUFFER_##commandType##_RES:                                \
  {                                                                       \
    resp = TrCommandBufferBase::CreateFromMessage<responseType>(message); \
    break;                                                                \
  }
      CASE_COMMAND_BUFFER_RESPONSE(WEBGL_CONTEXT_INIT, WebGL1ContextInitCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(WEBGL2_CONTEXT_INIT, WebGL2ContextInitCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_EXTENSIONS, GetExtensionsCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(LINK_PROGRAM, LinkProgramCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_PROGRAM_PARAM, GetProgramParamCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_PROGRAM_INFO_LOG, GetProgramInfoLogCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_SOURCE, GetShaderSourceCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_PARAM, GetShaderParamCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_INFO_LOG, GetShaderInfoLogCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(CHECK_FRAMEBUFFER_STATUS, CheckFramebufferStatusCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(IS_VERTEX_ARRAY, IsVertexArrayCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SAMPLER_PARAMETER, GetSamplerParameterCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(IS_SAMPLER, IsSamplerCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_BOOLEANV, GetBooleanvCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_INTEGERV, GetIntegervCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_FLOATV, GetFloatvCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_STRING, GetStringCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_SHADER_PRECISION_FORMAT, GetShaderPrecisionFormatCommandBufferResponse)
      CASE_COMMAND_BUFFER_RESPONSE(GET_ERROR, GetErrorCommandBufferResponse)
#undef CASE_COMMAND_BUFFER_RESPONSE
    default:
      DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
      break;
    }
    return resp;
  }
}
