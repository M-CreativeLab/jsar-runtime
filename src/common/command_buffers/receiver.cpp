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
#define XX(commandType, responseType)                                     \
  case COMMAND_BUFFER_##commandType##_RES:                                \
  {                                                                       \
    resp = TrCommandBufferBase::CreateFromMessage<responseType>(message); \
    break;                                                                \
  }
      TR_COMMAND_BUFFER_RESPONSES_MAP(XX)
#undef XX
    default:
      DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
      break;
    }
    return resp;
  }
}
