#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./event.hpp"
#include "./event_type.hpp"
#include "./message.hpp"

namespace events
{
  class TrEventSender : public ipc::TrChannelSender<TrEventMessage>
  {
  public:
    TrEventSender(ipc::TrOneShotClient<TrEventMessage> *client) : ipc::TrChannelSender<TrEventMessage>(client)
    {
    }

  public:
    bool sendEvent(TrEventMessage &event)
    {
      void *data;
      size_t size;
      if (!TrEventMessage::Serialize(event, &data, &size) || data == nullptr)
        return false;

      size_t headerSize = sizeof(TR_EVENT_MAGIC) + sizeof(size);
      size_t bufferSize = headerSize + size;
      char *buffer = (char *)malloc(bufferSize);

      // append header
      memcpy(buffer, &TR_EVENT_MAGIC, sizeof(TR_EVENT_MAGIC));
      memcpy(buffer + sizeof(TR_EVENT_MAGIC), &size, sizeof(size));

      // append data
      memcpy(buffer + headerSize, data, size);
      free(data);

      // send
      bool result = sendRaw(buffer, bufferSize);
      free(buffer);
      return result;
    }
  };
}
