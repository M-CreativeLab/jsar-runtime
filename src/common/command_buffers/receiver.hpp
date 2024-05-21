#pragma once

#include "./shared.hpp"
#include "./base.hpp"
#include "./message.hpp"

namespace commandbuffers
{
  class TrCommandBufferReceiver : public ipc::TrChannelReceiver<TrCommandBufferMessage>
  {
  public:
    TrCommandBufferReceiver(ipc::TrOneShotClient<TrCommandBufferMessage> *client) : ipc::TrChannelReceiver<TrCommandBufferMessage>(client) {}
    ~TrCommandBufferReceiver() {}

  public:
    TrCommandBufferBase *recvCommandBufferRequest(int timeout = 0);
    TrCommandBufferResponse *recvCommandBufferResponse(int timeout = 0);

    friend class TrCommandBufferMessage;
  };
}
