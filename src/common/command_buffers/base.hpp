#pragma once

#include "./shared.hpp"

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
    virtual ~TrCommandBufferBase() {}

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
    TrCommandBufferResponse(CommandBufferType type, TrCommandBufferResponse& that) : TrCommandBufferBase(type)
    {
      requestId = that.requestId;
    }
    TrCommandBufferResponse(CommandBufferType type, TrCommandBufferBase *req) : TrCommandBufferBase(type),
                                                                                requestId(req->id)
    {
    }

  public:
    int requestId;
  };

  template <typename T>
  class TrCommandBufferSimpleRequest : public TrCommandBufferBase
  {
  public:
    TrCommandBufferSimpleRequest(CommandBufferType type) : TrCommandBufferBase(type)
    {
      size = sizeof(T);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    inline void print()
    {
      DEBUG(LOG_TAG_RENDERER, "GL::%s()", commandTypeToStr(type).c_str());
    }
  };
}
