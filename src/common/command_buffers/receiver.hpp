#pragma once

#include "./shared.hpp"
#include "./base.hpp"

namespace commandbuffers
{
  class TrCommandBufferReceiver : public ipc::TrChannelReceiver<TrCommandBufferMessage>
  {
  public:
    TrCommandBufferReceiver(ipc::TrOneShotClient<TrCommandBufferMessage> *client) : ipc::TrChannelReceiver<TrCommandBufferMessage>(client) {}
    ~TrCommandBufferReceiver() {}

  public:
    // Receive a new command buffer request or response.
    //
    // It will return an allocated command buffer request object, the caller must manage its lifetime.
    [[nodiscard]] TrCommandBufferBase *recvCommandBufferRequest(int timeout = 0);
    [[nodiscard]] TrCommandBufferResponse *recvCommandBufferResponse(int timeout = 0);

    friend class TrCommandBufferMessage;
  };
}
