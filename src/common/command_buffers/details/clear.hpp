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
     * Serialize the command buffer to a JSON object with clear operation information.
     * Clear operations are important for understanding frame rendering pipeline.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus clear operation parameters
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information with new structure
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);

      // Add clear operation parameters to the parameters array in OpenGL function order
      rapidjson::Value &parameters = cmdInfo["parameters"];
      parameters.PushBack(rapidjson::Value().SetInt(mask), allocator);

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
     * Serialize the command buffer to a JSON object with clear color information.
     * Clear color is important for understanding the rendering background setup.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus clear color parameters
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information with new structure
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);

      // Add clear color parameters to the parameters array in OpenGL function order
      rapidjson::Value &parameters = cmdInfo["parameters"];
      parameters.PushBack(rapidjson::Value().SetFloat(r), allocator);
      parameters.PushBack(rapidjson::Value().SetFloat(g), allocator);
      parameters.PushBack(rapidjson::Value().SetFloat(b), allocator);
      parameters.PushBack(rapidjson::Value().SetFloat(a), allocator);

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
