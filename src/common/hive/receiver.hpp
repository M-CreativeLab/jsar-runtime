#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

namespace hive_comm
{
  class TrHiveCommandReceiver : public ipc::TrChannelReceiver<TrHiveCommandMessage>
  {
  public:
    TrHiveCommandReceiver(ipc::TrOneShotClient<TrHiveCommandMessage> *client)
        : ipc::TrChannelReceiver<TrHiveCommandMessage>(client)
    {
    }

  public:
    inline bool recvCommand(TrHiveCommandMessage &message, int recvTimeout)
    {
      return message.deserialize(this, recvTimeout);
    }
  };
}
