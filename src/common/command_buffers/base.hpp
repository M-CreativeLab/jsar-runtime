#pragma once

#include "./shared.hpp"
#include "./message.hpp"

namespace commandbuffers
{
  class TrCommandBufferBase
  {
  public:
    template <typename T>
    static T *CreateFromMessage(TrCommandBufferMessage &message)
    {
      T *commandBuffer = message.createInstanceFromBase<T>();
      commandBuffer->deserialize(message);
      return commandBuffer;
    }

  public:
    TrCommandBufferBase() {}
    TrCommandBufferBase(CommandBufferType type) : type(type) {}

  public:
    virtual TrCommandBufferMessage *serialize() = 0;
    virtual void deserialize(TrCommandBufferMessage &message) = 0;

  public:
    CommandBufferType type = COMMAND_BUFFER_UNKNOWN;
    uint32_t id = commandBufferIdGen.get();
    size_t size = 0;
  };

  class TrCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    TrCommandBufferResponse(CommandBufferType type, TrCommandBufferBase *req) : TrCommandBufferBase(type),
                                                                                requestId(req->id)
    {
    }

  public:
    int requestId;
  };
}
