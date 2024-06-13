#pragma once

#include "./shared.hpp"

namespace commandbuffers
{
  class TrCommandBufferBase : public ipc::TrIpcSerializableBase<TrCommandBufferMessage, CommandBufferType>
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
    TrCommandBufferBase() : TrIpcSerializableBase() {}
    TrCommandBufferBase(CommandBufferType type, size_t size = 0) : TrIpcSerializableBase(type, size) {}

  public:
    uint32_t id = commandBufferIdGen.get();
  };

  using TrCommandBufferRequest = TrCommandBufferBase;
  class TrCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    TrCommandBufferResponse(CommandBufferType type, TrCommandBufferResponse &that)
        : TrCommandBufferBase(type, that.size),
          requestId(that.requestId)
    {
    }
    TrCommandBufferResponse(CommandBufferType type, TrCommandBufferRequest *req)
        : TrCommandBufferBase(type),
          requestId(req->id)
    {
    }

  public:
    int requestId;
  };

  template <typename T>
  class TrCommandBufferSimpleRequest : public TrCommandBufferRequest
  {
  public:
    TrCommandBufferSimpleRequest(CommandBufferType type) : TrCommandBufferRequest(type, sizeof(T))
    {
    }

  public:
    inline void print()
    {
      DEBUG(LOG_TAG_RENDERER, "GL::%s()", commandTypeToStr(type).c_str());
    }

  public:
    size_t size = sizeof(T);
  };

  template <typename T>
  class TrCommandBufferSimpleResponse : public TrCommandBufferResponse
  {
  public:
    TrCommandBufferSimpleResponse(CommandBufferType type, TrCommandBufferResponse &that)
        : TrCommandBufferResponse(type, that)
    {
    }
    TrCommandBufferSimpleResponse(CommandBufferType type, TrCommandBufferRequest *req)
        : TrCommandBufferResponse(type, req)
    {
    }
  };
}
