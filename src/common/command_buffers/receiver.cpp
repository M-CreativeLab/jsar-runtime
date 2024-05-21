#include "./receiver.hpp"
#include "./command_buffers.hpp"

namespace commandbuffers
{
  TrCommandBufferBase *TrCommandBufferReceiver::recvCommandBufferRequest(int timeout)
  {
    TrCommandBufferMessage message;
    if (!message.deserialize(this, timeout))
      return nullptr;

    if (message.type == COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ)
    {
      return TrCommandBufferBase::CreateFromMessage<WebGL1ContextInitCommandBufferRequest>(message);
    }
    else
    {
      DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
      return nullptr;
    }
  }

  TrCommandBufferResponse *TrCommandBufferReceiver::recvCommandBufferResponse(int timeout)
  {
    TrCommandBufferMessage message;
    if (!message.deserialize(this, timeout))
      return nullptr;

    if (message.type == COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES)
    {
      return TrCommandBufferBase::CreateFromMessage<WebGL1ContextInitCommandBufferResponse>(message);
    }
    else
    {
      DEBUG(LOG_TAG_CONTENT, "Received an unknown command buffer: %d", message.type);
      return nullptr;
    }
  }
}
