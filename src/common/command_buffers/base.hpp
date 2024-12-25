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

  class TrCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TrCommandBufferRequest(TrCommandBufferRequest &that)
        : TrCommandBufferBase(that.type, that.size)
    {
      renderingInfo = that.renderingInfo;
    }
    TrCommandBufferRequest(CommandBufferType type, size_t size)
        : TrCommandBufferBase(type, size)
    {
    }
  };

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

  /**
   * A convienent template class to define the exchange-able command buffer request.
   *
   * @tparam Derived The derived class of the request.
   * @tparam Type The type of the command buffer, which should be a request command buffer type.
   */
  template <typename Derived, CommandBufferType Type>
    requires is_commandbuffer_request<Type>
  class TrCommandBufferSimpleRequest : public TrCommandBufferRequest
  {
  public:
    TrCommandBufferSimpleRequest()
        : TrCommandBufferRequest(Type, sizeof(Derived))
    {
    }
    TrCommandBufferSimpleRequest(TrCommandBufferSimpleRequest &that)
        : TrCommandBufferRequest(that)
    {
    }
    ~TrCommandBufferSimpleRequest() = default;

  public:
    inline void print()
    {
      DEBUG(LOG_TAG_RENDERER, "GL::%s()", commandTypeToStr(Type).c_str());
    }
  };

  /**
   * A convienent template class to define the exchange-able command buffer response.
   *
   * @tparam Derived The derived class of the response.
   * @todo Support template parameter `Type` to specify the type of the command buffer.
   */
  template <typename Derived>
  class TrCommandBufferSimpleResponse : public TrCommandBufferResponse
  {
  public:
    TrCommandBufferSimpleResponse(TrCommandBufferSimpleResponse &that) : TrCommandBufferResponse(that) {}
    TrCommandBufferSimpleResponse(CommandBufferType type, TrCommandBufferRequest *req)
        : TrCommandBufferResponse(type, sizeof(Derived), req)
    {
    }
  };
}
