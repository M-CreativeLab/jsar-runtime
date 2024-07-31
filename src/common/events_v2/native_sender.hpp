#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./native_event.hpp"
#include "./native_message.hpp"

namespace events_comm
{
  class TrNativeEventSender : public ipc::TrChannelSender<TrNativeEventMessage>
  {
  public:
    TrNativeEventSender(ipc::TrOneShotClient<TrNativeEventMessage> *client)
        : ipc::TrChannelSender<TrNativeEventMessage>(client)
    {
    }

  public:
    bool dispatchEvent(TrNativeEvent &event, uint32_t peerId = 0)
    {
      TrNativeEventMessage *message = nullptr;
      switch (event.type)
      {
#define XX(eventType)                                           \
  case TrNativeEventType::eventType:                            \
  {                                                             \
    message = Tr##eventType##Remote(event, peerId).serialize(); \
    break;                                                      \
  }
        TR_NATIVE_EVENTS_MAP(XX)
#undef XX
      default:
        break;
      };

      if (TR_UNLIKELY(message == nullptr))
      {
        DEBUG(LOG_TAG_CONTENT, "Failed to serialize a NativeEvent(0x%x)", event.type);
        return false;
      }

      void *data = nullptr;
      size_t size = 0;
      auto success = message->serialize(&data, &size);
      delete message;
      if (!success)
        return false;

      auto r = sendRaw(data, size);
      free(data);
      return r;
    }
  };
}
