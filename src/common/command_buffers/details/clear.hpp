#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class ClearCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearCommandBufferRequest, COMMAND_BUFFER_CLEAR_REQ>
  {
  public:
    ClearCommandBufferRequest() = delete;
    ClearCommandBufferRequest(int mask)
        : TrCommandBufferSimpleRequest()
        , mask(mask)
    {
    }
    ClearCommandBufferRequest(const ClearCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mask(that.mask)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << mask << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed clear operation information.
     * Clear operations are important for understanding frame rendering pipeline.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus clear operation details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add clear operation specific details
      rapidjson::Value clearDetails(rapidjson::kObjectType);
      clearDetails.AddMember("mask", rapidjson::Value().SetInt(mask), allocator);
      clearDetails.AddMember("operation", rapidjson::Value().SetString("clear", allocator), allocator);
      
      // Decode the clear mask for better understanding
      rapidjson::Value maskComponents(rapidjson::kArrayType);
      if (mask & 0x4000) maskComponents.PushBack(rapidjson::Value().SetString("COLOR_BUFFER_BIT", allocator), allocator);
      if (mask & 0x0100) maskComponents.PushBack(rapidjson::Value().SetString("DEPTH_BUFFER_BIT", allocator), allocator);
      if (mask & 0x0400) maskComponents.PushBack(rapidjson::Value().SetString("STENCIL_BUFFER_BIT", allocator), allocator);
      clearDetails.AddMember("maskComponents", maskComponents, allocator);
      
      cmdInfo.AddMember("clearDetails", clearDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int mask;
  };

  class ClearColorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearColorCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_COLOR_REQ>
  {
  public:
    ClearColorCommandBufferRequest() = delete;
    ClearColorCommandBufferRequest(float r, float g, float b, float a)
        : TrCommandBufferSimpleRequest()
        , r(r)
        , g(g)
        , b(b)
        , a(a)
    {
    }
    ClearColorCommandBufferRequest(const ClearColorCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , r(that.r)
        , g(that.g)
        , b(that.b)
        , a(that.a)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << r << ", "
         << g << ", "
         << b << ", "
         << a << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed clear color information.
     * Clear color is important for understanding the rendering background setup.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus clear color details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add clear color specific details
      rapidjson::Value clearDetails(rapidjson::kObjectType);
      clearDetails.AddMember("r", rapidjson::Value().SetFloat(r), allocator);
      clearDetails.AddMember("g", rapidjson::Value().SetFloat(g), allocator);
      clearDetails.AddMember("b", rapidjson::Value().SetFloat(b), allocator);
      clearDetails.AddMember("a", rapidjson::Value().SetFloat(a), allocator);
      clearDetails.AddMember("operation", rapidjson::Value().SetString("clearColor", allocator), allocator);
      
      // Add color as hex string for easier reading
      char hexColor[32];
      snprintf(hexColor, sizeof(hexColor), "#%02X%02X%02X%02X", 
               (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
      clearDetails.AddMember("colorHex", rapidjson::Value().SetString(hexColor, allocator), allocator);
      
      cmdInfo.AddMember("clearDetails", clearDetails, allocator);
      
      return cmdInfo;
    }

  public:
    float r;
    float g;
    float b;
    float a;
  };

  class ClearDepthCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearDepthCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_DEPTH_REQ>
  {
  public:
    ClearDepthCommandBufferRequest() = delete;
    ClearDepthCommandBufferRequest(float depth)
        : TrCommandBufferSimpleRequest()
        , depth(depth)
    {
    }
    ClearDepthCommandBufferRequest(const ClearDepthCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , depth(that.depth)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << depth << ")";
      return ss.str();
    }

  public:
    float depth;
  };

  class ClearStencilCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearStencilCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_STENCIL_REQ>
  {
  public:
    ClearStencilCommandBufferRequest() = delete;
    ClearStencilCommandBufferRequest(int stencil)
        : TrCommandBufferSimpleRequest()
        , stencil(stencil)
    {
    }
    ClearStencilCommandBufferRequest(const ClearStencilCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , stencil(that.stencil)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << stencil << ")";
      return ss.str();
    }

  public:
    int stencil;
  };
}
