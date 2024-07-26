#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./message.hpp"

namespace media_comm
{
  class TrMediaCommandReceiver : public ipc::TrChannelReceiver<TrMediaCommandMessage>
  {
  public:
    TrMediaCommandReceiver(ipc::TrOneShotClient<TrMediaCommandMessage> *client)
        : ipc::TrChannelReceiver<TrMediaCommandMessage>(client)
    {
    }

  public:
    inline bool recvCommand(TrMediaCommandMessage &message, int recvTimeout)
    {
      return message.deserialize(this, recvTimeout);
    }
  };
}
