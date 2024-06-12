#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"
#include "./types.hpp"

namespace xr
{
  class TrXRCommandReceiver : public ipc::TrChannelReceiver<TrXRCommandMessage>
  {
  public:
    TrXRCommandReceiver(ipc::TrOneShotClient<TrXRCommandMessage> *client) : ipc::TrChannelReceiver<TrXRCommandMessage>(client)
    {
    }

  public:
    /**
     * It receives a command message from the client with a timeout. Once the message is received, it will be deserialized and
     * create a `TrXRCommandMessage` instance, the caller is responsible for freeing the memory of this instance.
     */
    TrXRCommandMessage *recvCommandMessage(int recvTimeout)
    {
      int16_t magic;
      if (!tryRecvRaw(&magic, sizeof(magic), recvTimeout) || magic != TR_XRCOMMAND_MAGIC)
        return nullptr;

      xr::TrXRCmdType type;
      if (!tryRecvRaw(&type, sizeof(type), recvTimeout))
        return nullptr;

      size_t dataSize;
      if (!tryRecvRaw(&dataSize, sizeof(dataSize), recvTimeout))
        return nullptr;

      auto data = (char *)malloc(dataSize);
      if (!tryRecvRaw(data, dataSize, recvTimeout))
        return nullptr;

      TrXRCommandMessage *xrCommandMessage = new TrXRCommandMessage(type, data, dataSize);
      free(data);
      return xrCommandMessage;
    }
  };
}
