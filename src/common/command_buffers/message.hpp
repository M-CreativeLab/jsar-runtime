#pragma once

#include "./shared.hpp"

namespace commandbuffers
{
  using namespace ipc;

  class TrCommandBufferMessage : public TrIpcMessage<TrCommandBufferMessage, CommandBufferType>
  {
  public:
    TrCommandBufferMessage()
        : TrIpcMessage(COMMAND_BUFFER_UNKNOWN, 0, nullptr)
    {
    }
    TrCommandBufferMessage(CommandBufferType type, size_t size, void *base)
        : TrIpcMessage(type, size, base)
    {
    }
  
    friend class TrCommandBufferBase;
    friend class TrCommandBufferSender;
    friend class TrCommandBufferReceiver;
  };
}
