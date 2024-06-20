#include "./sender.hpp"
#include "./command_buffers.hpp"
#include "./macros.hpp"

namespace commandbuffers
{

  bool TrCommandBufferSender::sendCommandBufferRequest(TrCommandBufferBase &req)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (req.type)
    {
#define XX(commandType, requestType)                          \
  case COMMAND_BUFFER_##commandType##_REQ:                    \
  {                                                           \
    message = dynamic_cast<requestType *>(&req)->serialize(); \
    break;                                                    \
  }
      TR_COMMAND_BUFFER_REQUESTS_MAP(XX)
#undef XX
    default:
      break;
    };

    if (message == nullptr)
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", req.type);
      return false;
    }

    void *data = nullptr;
    size_t size = 0;
    auto success = message->serialize(&data, &size);
    delete message;
    if (!success)
    {
      assert(data == nullptr);  // !success means allocation failure.
      return false;
    }

    assert(data != nullptr && size > 0);
    auto r = sendRaw(data, size);
    free(data);
    return r;
  }

  bool TrCommandBufferSender::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (res.type)
    {
#define CASE_COMMAND_BUFFER_RESPONSE(commandType, responseType) \
  case COMMAND_BUFFER_##commandType##_RES:                      \
  {                                                             \
    message = dynamic_cast<responseType *>(&res)->serialize();  \
    break;                                                      \
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
      break;
    };

    if (message == nullptr)
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize command buffer: %d", res.type);
      return false;
    }

    void *data = nullptr;
    size_t size = 0;
    bool success = message->serialize(&data, &size);
    delete message;
    if (!success)
    {
      assert(data == nullptr);
      return false;
    }

    assert(data != nullptr && size > 0);
    auto r = sendRaw(data, size);
    free(data);
    return r;
  }
}
