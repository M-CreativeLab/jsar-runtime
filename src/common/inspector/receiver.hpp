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
     * It receives a command message from the client with a timeout using the message's built-in deserialization.
     */
    inline bool recvCommand(TrInspectorCommandMessage &message, int recvTimeout)
    {
      return message.deserialize(this, recvTimeout);
    }
  };
}