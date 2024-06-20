#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./event.hpp"
#include "./event_type.hpp"
#include "./message.hpp"

namespace events
{
  class TrEventReceiver : public ipc::TrChannelReceiver<TrEventMessage>
  {
  public:
    TrEventReceiver(ipc::TrOneShotClient<TrEventMessage> *client) : ipc::TrChannelReceiver<TrEventMessage>(client)
    {
    }

  public:
    TrEventMessage *recvEvent(int recvTimeout)
    {
      int16_t magic;
      if (!tryRecvRaw(&magic, sizeof(magic), recvTimeout) || magic != TR_EVENT_MAGIC)
        return nullptr;

      size_t dataSize;
      if (!tryRecvRaw(&dataSize, sizeof(dataSize), recvTimeout))
        return nullptr;

      auto data = (char *)malloc(dataSize);
      if (data == nullptr)
        return nullptr;

      if (!tryRecvRaw(data, dataSize, recvTimeout))
      {
        free(data);
        return nullptr;
      }

      TrEventMessage *event = TrEventMessage::Deserialize(data, dataSize);
      free(data);
      return event;
    }
  };
}
