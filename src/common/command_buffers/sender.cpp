#include "./sender.hpp"
#include "./command_buffers.hpp"
#include "./macros.hpp"

namespace commandbuffers
{
  bool TrCommandBufferSender::sendCommandBufferRequest(TrCommandBufferBase &req, bool forceFlush)
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

    if (TR_UNLIKELY(message == nullptr))
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize a CommandBufferRequest(%d)", req.type);
      return false;
    }

    void *data = nullptr;
    size_t size = 0;
    auto success = message->serialize(&data, &size);
    delete message;
    if (!success)
    {
      assert(data == nullptr); // !success means allocation failure.
      return false;
    }

    assert(data != nullptr && size > 0);
    auto r = enqueue(data, size);
    if (forceFlush || needFlush())
      flush();
    return r;
  }

  bool TrCommandBufferSender::sendCommandBufferResponse(TrCommandBufferResponse &res)
  {
    TrCommandBufferMessage *message = nullptr;
    switch (res.type)
    {
#define XX(commandType, responseType)                          \
  case COMMAND_BUFFER_##commandType##_RES:                     \
  {                                                            \
    message = dynamic_cast<responseType *>(&res)->serialize(); \
    break;                                                     \
  }
      TR_COMMAND_BUFFER_RESPONSES_MAP(XX)
#undef XX
    default:
      break;
    };

    if (message == nullptr)
    {
      DEBUG(LOG_TAG_CONTENT, "Failed to serialize a CommandBufferResponse(%d)", res.type);
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
