#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

using namespace std;
using namespace ipc;

namespace inspector_comm
{
  class TrInspectorReceiver : public ipc::TrChannelReceiver<TrInspectorCommandMessage>
  {
  public:
    TrInspectorReceiver(ipc::TrOneShotClient<TrInspectorCommandMessage> *client)
        : ipc::TrChannelReceiver<TrInspectorCommandMessage>(client)
    {
    }

  public:
    /**
     * It receives a command message from the client with a timeout. Once the message is received, it will be deserialized and
     * create a `TrInspectorCommandMessage` instance, the caller is responsible for freeing the memory of this instance.
     */
    TrInspectorCommandMessage *recvCommandMessage(int recvTimeout)
    {
      int16_t magic;
      if (!tryRecvRaw(&magic, sizeof(magic), recvTimeout) || magic != TR_INSPECTOR_MAGIC)
        return nullptr;

      inspector_comm::TrInspectorCommandType type;
      if (!tryRecvRaw(&type, sizeof(type), recvTimeout))
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

      TrInspectorCommandMessage *inspectorCommandMessage = new TrInspectorCommandMessage(type, dataSize, data);
      free(data);
      return inspectorCommandMessage;
    }
  };
}