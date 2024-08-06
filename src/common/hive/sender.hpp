#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

namespace hive_comm
{
  class TrHiveCommandSender : public ipc::TrChannelSender<TrHiveCommandMessage>
  {
  public:
    TrHiveCommandSender(ipc::TrOneShotClient<TrHiveCommandMessage> *client)
        : ipc::TrChannelSender<TrHiveCommandMessage>(client)
    {
    }

  public:
    bool sendCommand(TrHiveCommandBase &command)
    {
      TrHiveCommandMessage *message = nullptr;
      switch (command.type)
      {
#define XX(commandType)                                               \
  case TrHiveCommandType::commandType:                                \
  {                                                                   \
    message = dynamic_cast<Tr##commandType *>(&command)->serialize(); \
    break;                                                            \
  }
        TR_HIVE_COMMANDS_MAP(XX)
#undef XX
      default:
        break;
      };

      if (TR_UNLIKELY(message == nullptr))
      {
        DEBUG(LOG_TAG_CONTENT, "Failed to serialize HiveCommand(%d)", command.type);
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
