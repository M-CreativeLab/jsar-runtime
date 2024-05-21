#pragma once

#include "./shared.hpp"
#include "./base.hpp"
#include "./message.hpp"

namespace commandbuffers
{
  class TrCommandBufferSender : public ipc::TrChannelSender<TrCommandBufferMessage>
  {
  public:
    TrCommandBufferSender(ipc::TrOneShotClient<TrCommandBufferMessage> *client) : ipc::TrChannelSender<TrCommandBufferMessage>(client) {}
    ~TrCommandBufferSender() {}

  public:
    bool sendCommandBufferRequest(TrCommandBufferBase &req);
    bool sendCommandBufferResponse(TrCommandBufferResponse &res);
  };
}
