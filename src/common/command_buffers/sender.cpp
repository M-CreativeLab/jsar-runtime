#include "./sender.hpp"
#include "./command_buffers.hpp"

namespace commandbuffers
{

  bool TrCommandBufferSender::sendCommandBufferRequest(TrCommandBufferBase &req)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (req.type)
    {
    case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_REQ:
    {
      WebGL1ContextInitCommandBufferRequest *data = dynamic_cast<WebGL1ContextInitCommandBufferRequest *>(&req);
      message = data->serialize();
      break;
    }
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
    if (!message->serialize(&data, &size))
      return false;

    return sendRaw(data, size);
  }

  bool TrCommandBufferSender::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (res.type)
    {
    case COMMAND_BUFFER_WEBGL_CONTEXT_INIT_RES:
    {
      auto data = dynamic_cast<WebGL1ContextInitCommandBufferResponse *>(&res);
      message = data->serialize();
      break;
    }
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
    if (!message->serialize(&data, &size))
      return false;

    return sendRaw(data, size);
  }
}
