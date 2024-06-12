#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"
#include "./types.hpp"

namespace xr
{
  class TrXRCommandSender : public ipc::TrChannelSender<TrXRCommandMessage>
  {
  public:
    TrXRCommandSender(ipc::TrOneShotClient<TrXRCommandMessage> *client) : ipc::TrChannelSender<TrXRCommandMessage>(client)
    {
    }

  public:
    template <typename CommandType>
    bool sendCommand(TrXRCommandBase<CommandType> &xrCommand)
    {
      TrXRCommandMessage *xrCommandMessage = TrXRCommandMessage::Make(xrCommand);
      void *data = xrCommandMessage->base;
      size_t size = xrCommandMessage->baseSize;

      size_t headerSize = sizeof(TR_XRCOMMAND_MAGIC)       // magic
                          + sizeof(xrCommandMessage->type) // command type
                          + sizeof(size);                  // base size
      size_t bufferSize = headerSize + size;
      char *buffer = (char *)malloc(bufferSize);

      // append header
      int offset = 0;
      memcpy(buffer + offset, &TR_XRCOMMAND_MAGIC, sizeof(TR_XRCOMMAND_MAGIC));
      offset += sizeof(TR_XRCOMMAND_MAGIC);
      memcpy(buffer + offset, &xrCommandMessage->type, sizeof(xrCommandMessage->type));
      offset += sizeof(xrCommandMessage->type);
      memcpy(buffer + offset, &size, sizeof(size));

      // append data
      memcpy(buffer + headerSize, data, size);
      delete xrCommandMessage;
      xrCommandMessage = nullptr;
      data = nullptr;

      // send
      bool result = sendRaw(buffer, bufferSize);
      free(buffer);
      return result;
    }
  };
}
