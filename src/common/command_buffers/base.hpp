#pragma once

#include "./shared.hpp"
#include "../xr/types.hpp"

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
    TrCommandBufferBase(CommandBufferType type, size_t size) : TrIpcSerializableBase(type, size) {}

  public:
    uint32_t id = commandBufferIdGen.get();
    xr::TrXRFrameRenderingInfo renderingInfo;
  };

  using TrCommandBufferRequest = TrCommandBufferBase;
  class TrCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    TrCommandBufferResponse(TrCommandBufferResponse &that)
        : TrCommandBufferBase(that.type, that.size),
          requestId(that.requestId)
    {
    }
    TrCommandBufferResponse(CommandBufferType type, size_t size, TrCommandBufferRequest *req)
        : TrCommandBufferBase(type, size),
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
    TrCommandBufferSimpleRequest(CommandBufferType type)
        : TrCommandBufferRequest(type, sizeof(T))
    {
    }

  public:
    inline void print()
    {
      DEBUG(LOG_TAG_RENDERER, "GL::%s()", commandTypeToStr(type).c_str());
    }
  };

  template <typename T>
  class TrCommandBufferSimpleResponse : public TrCommandBufferResponse
  {
  public:
    TrCommandBufferSimpleResponse(TrCommandBufferSimpleResponse &that) : TrCommandBufferResponse(that) {}
    TrCommandBufferSimpleResponse(CommandBufferType type, TrCommandBufferRequest *req)
        : TrCommandBufferResponse(type, sizeof(T), req)
    {
    }
  };
}
