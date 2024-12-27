#pragma once

#include "./shared.hpp"
#include "../xr/types.hpp"

namespace commandbuffers
{
  /**
   * MCI: The Minimum Context Id.
   *
   * Every commandbuffer request or response should have a context Id, which is used to identify the context of the command
   * buffer in a same content process.
   *
   * Invalid context Id is less than this value, and the default context Id is this value, that indicates this context is the
   * first created commandbuffers context in the content process.
   */
  constexpr uint8_t MinimumContextId = 0xd;

  /**
   * The maximum count of contexts to be created per content process.
   *
   * TODO: This value should be configurable?
   */
  constexpr uint32_t MaxinumContextsCountPerContent = 5;

  /**
   * The base struct/class to represent the exchange-able command buffer.
   */
  class TrCommandBufferBase : public ipc::TrIpcSerializableBase<TrCommandBufferMessage, CommandBufferType>
  {
#define INVALID_CONTEXT_ID 0x0

  public:
    /**
     * Create a command buffer from an `TrCommandBufferMessage` instance.
     *
     * @tparam T The command buffer type of the instance to be created.
     * @param message The message to create the command buffer instance.
     * @returns The created command buffer instance.
     */
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
    /**
     * The Id of the command buffer.
     *
     * Every command buffer should have a unique Id to identify the command buffer which is generated once the command buffer
     * is created at both the client or server side.
     */
    uint32_t id = commandBufferIdGen.get();
    /**
     * The context Id of the command buffer, that is used to identify the context of the command buffer in a same content
     * process.
     *
     * This field is initialized to be `MinimumContextId - 1` by default, which indicates this context is invalid. The sender
     * in the client side or server side should set this field to the correct context Id before sending the command buffer.
     */
    uint8_t contextId = INVALID_CONTEXT_ID; // Mark this to be invalid by default.
    /**
     * The WebXR rendering information of this command buffer, which is used to identify if this command buffer is belongs to
     * a WebXR frame rendering or not.
     */
    xr::TrXRFrameRenderingInfo renderingInfo;
#undef INVALID_CONTEXT_ID
  };

  class TrCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    TrCommandBufferRequest(CommandBufferType type, size_t size)
        : TrCommandBufferBase(type, size)
    {
    }
    TrCommandBufferRequest(TrCommandBufferRequest &that)
        : TrCommandBufferBase(that.type, that.size)
    {
      contextId = that.contextId;
      renderingInfo = that.renderingInfo;
    }
  };

  class TrCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    TrCommandBufferResponse(CommandBufferType type, size_t size, TrCommandBufferRequest *req)
        : TrCommandBufferBase(type, size),
          requestId(req->id)
    {
    }
    TrCommandBufferResponse(TrCommandBufferResponse &that)
        : TrCommandBufferBase(that.type, that.size),
          requestId(that.requestId)
    {
      contextId = that.contextId;
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
