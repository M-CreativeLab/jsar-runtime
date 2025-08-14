#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

namespace inspector_comm
{
  class TrInspectorCommandSender : public ipc::TrChannelSender<TrInspectorCommandMessage>
  {
  public:
    TrInspectorCommandSender(ipc::TrOneShotClient<TrInspectorCommandMessage> *client)
        : ipc::TrChannelSender<TrInspectorCommandMessage>(client)
    {
    }

  public:
    bool sendCommand(TrInspectorCommandBase &command)
    {
      TrInspectorCommandMessage *message = nullptr;
      switch (command.type)
      {
#define XX(commandType)                                               \
  case TrInspectorCommandType::commandType:                           \
  {                                                                   \
    message = dynamic_cast<Tr##commandType *>(&command)->serialize(); \
    break;                                                            \
  }
        TR_INSPECTOR_COMMANDS_MAP(XX)
#undef XX
      default:
        break;
      };

      if (TR_UNLIKELY(message == nullptr))
      {
        DEBUG(LOG_TAG_CONTENT, "Failed to serialize InspectorCommand(%d)", command.type);
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