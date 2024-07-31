#pragma once

#include <stdlib.h>

#include "common/ipc.hpp"
#include "./native_event.hpp"
#include "./native_message.hpp"

namespace events_comm
{
  class TrNativeEventReceiver : public ipc::TrChannelReceiver<TrNativeEventMessage>
  {
  public:
    TrNativeEventReceiver(ipc::TrOneShotClient<TrNativeEventMessage> *client)
        : ipc::TrChannelReceiver<TrNativeEventMessage>(client)
    {
    }

  public:
    /**
     * Try to receive an event message, it will returns a new instance you should delete it after using.
     */
    inline TrNativeEventMessage* recvEvent(int timeout)
    {
      TrNativeEventMessage* newMessage = new TrNativeEventMessage();
      if (!newMessage->deserialize(this, timeout))
      {
        delete newMessage;
        return nullptr;
      }
      return newMessage;
    }
    /**
     * Try to receive an event message, compare to `recvEvent`, this method will do the deserialization on the given message, it's better
     * to use this method if you want to reuse the message instance.
     */
    inline bool recvEventOn(TrNativeEventMessage &message, int timeout)
    {
      return message.deserialize(this, timeout);
    }
  };
}
