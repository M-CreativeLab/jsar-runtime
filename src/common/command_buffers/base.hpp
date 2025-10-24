#pragma once

#include <iostream>
#include <string>
#include <rapidjson/document.h>

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
    TrCommandBufferBase()
        : TrIpcSerializableBase()
    {
    }
    TrCommandBufferBase(CommandBufferType type, size_t size)
        : TrIpcSerializableBase(type, size)
    {
    }
    TrCommandBufferBase(const TrCommandBufferBase &that)
        : TrIpcSerializableBase(that.type, that.size)
        , id(that.id)
        , contextId(that.contextId)
        , renderingInfo(that.renderingInfo)
    {
    }

  public:
    virtual std::string toString(const char *prefix = "") const
    {
      return "";
    }

    /**
     * Serialize the command buffer to a JSON object.
     * This method provides structured data for debugging and inspection tools.
     * 
     * ## JSON Structure
     * The implementation provides:
     * - `id`: Command buffer unique identifier
     * - `type`: Numeric command type (see CommandBufferType enum)
     * - `typeName`: Human-readable command type name
     * - `parameters`: Object containing the GLES call parameters
     * - `context`: Object containing extra context info such as current program, VAO, etc.
     *   - `contextId`: WebGL context identifier  
     *   - `xrRenderingInfo`: XR session information (if applicable)
     *     - `sessionId`: XR session identifier
     *     - `stereoId`: Stereo rendering identifier
     *     - `viewIndex`: Eye/view index for XR rendering
     * 
     * Derived classes should call this base method and add command-specific parameters
     * to the "parameters" object returned by this method.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object representing the command buffer
     */
    virtual rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const
    {
      rapidjson::Value cmdInfo(rapidjson::kObjectType);

      // Basic command buffer information at top level
      cmdInfo.AddMember("id", rapidjson::Value().SetUint(id), allocator);
      cmdInfo.AddMember("type", rapidjson::Value().SetInt(static_cast<int>(type)), allocator);

      // Command type name for human readability
      std::string typeName = commandTypeToStr(type);
      cmdInfo.AddMember("typeName", rapidjson::Value().SetString(typeName.c_str(), allocator), allocator);

      // Create empty parameters array - derived classes will populate this
      rapidjson::Value parameters(rapidjson::kArrayType);
      cmdInfo.AddMember("parameters", parameters, allocator);

      // Create context object with contextId and XR info
      rapidjson::Value context(rapidjson::kObjectType);
      context.AddMember("contextId", rapidjson::Value().SetUint(contextId), allocator);

      // XR rendering info if available
      if (renderingInfo.isValid())
      {
        rapidjson::Value xrInfo(rapidjson::kObjectType);
        xrInfo.AddMember("sessionId", rapidjson::Value().SetUint(renderingInfo.sessionId), allocator);
        xrInfo.AddMember("stereoId", rapidjson::Value().SetUint(renderingInfo.stereoId), allocator);
        xrInfo.AddMember("viewIndex", rapidjson::Value().SetInt(renderingInfo.viewIndex), allocator);
        context.AddMember("xrRenderingInfo", xrInfo, allocator);
      }

      cmdInfo.AddMember("context", context, allocator);

      return cmdInfo;
    }

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
    TrCommandBufferRequest(const TrCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferBase(that)
    {
    }

  public:
    virtual std::string toString(const char *_ /* ignore prefix in request */ = nullptr) const override
    {
      return commandTypeToStr(this->type);
    }
  };

  class TrCommandBufferResponse : public TrCommandBufferBase
  {
  public:
    TrCommandBufferResponse(CommandBufferType type, size_t size, TrCommandBufferRequest *req)
        : TrCommandBufferBase(type, size)
        , requestId(req->id)
    {
      contextId = req->contextId;
    }
    TrCommandBufferResponse(const TrCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferBase(that)
        , requestId(that.requestId)
    {
    }

  public:
    virtual std::string toString(const char *prefix = "") const override
    {
      std::stringstream ss;
      ss << prefix << commandTypeToStr(this->type);
      return ss.str();
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
    TrCommandBufferSimpleRequest(const TrCommandBufferSimpleRequest &that, bool clone = false)
        : TrCommandBufferRequest(that, clone)
    {
    }
    ~TrCommandBufferSimpleRequest() = default;
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
    TrCommandBufferSimpleResponse(CommandBufferType type, TrCommandBufferRequest *req)
        : TrCommandBufferResponse(type, sizeof(Derived), req)
    {
    }
    TrCommandBufferSimpleResponse(const TrCommandBufferSimpleResponse &that, bool clone = false)
        : TrCommandBufferResponse(that, clone)
    {
    }
  };
}
