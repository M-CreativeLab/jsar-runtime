#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

namespace media_comm
{
  class TrMediaCommandSender : public ipc::TrChannelSender<TrMediaCommandMessage>
  {
  public:
    TrMediaCommandSender(ipc::TrOneShotClient<TrMediaCommandMessage> *client)
        : ipc::TrChannelSender<TrMediaCommandMessage>(client)
    {
    }

  public:
    bool sendCommand(TrMediaCommandBase &command)
    {
      TrMediaCommandMessage *message = nullptr;
      switch (command.type)
      {
#define XX(commandType)                                               \
  case TrMediaCommandType::commandType:                               \
  {                                                                   \
    message = dynamic_cast<Tr##commandType *>(&command)->serialize(); \
    break;                                                            \
  }
        TR_MEDIA_COMMANDS_MAP(XX)
#undef XX
      default:
        break;
      };

      if (TR_UNLIKELY(message == nullptr))
      {
        DEBUG(LOG_TAG_CONTENT, "Failed to serialize MediaCommand(%d)", command.type);
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
