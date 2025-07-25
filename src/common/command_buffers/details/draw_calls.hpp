#pragma once

#include <common/command_buffers/webgl_constants.hpp>

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class DrawArraysCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawArraysCommandBufferRequest, COMMAND_BUFFER_DRAW_ARRAYS_REQ>
  {
  public:
    DrawArraysCommandBufferRequest() = delete;
    DrawArraysCommandBufferRequest(int mode, int first, int count)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
        , first(first)
        , count(count)
    {
    }
    DrawArraysCommandBufferRequest(const DrawArraysCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
        , first(that.first)
        , count(that.count)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << "GL::DrawArrays(mode=" << mode << ", "
         << first << ", "
         << count << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed draw arrays information.
     * This is crucial for GPU profiling as draw calls are major performance points.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus draw arrays details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add draw arrays specific details
      rapidjson::Value drawDetails(rapidjson::kObjectType);
      drawDetails.AddMember("mode", rapidjson::Value().SetInt(mode), allocator);
      drawDetails.AddMember("first", rapidjson::Value().SetInt(first), allocator);
      drawDetails.AddMember("count", rapidjson::Value().SetInt(count), allocator);
      drawDetails.AddMember("operation", rapidjson::Value().SetString("drawArrays", allocator), allocator);
      drawDetails.AddMember("primitiveCount", rapidjson::Value().SetInt(count), allocator);
      
      cmdInfo.AddMember("drawDetails", drawDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int mode;
    int first;
    int count;
  };

  class DrawArraysInstancedCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawArraysInstancedCommandBufferRequest,
                                            COMMAND_BUFFER_DRAW_ARRAYS_INSTANCED_REQ>
  {
  public:
    DrawArraysInstancedCommandBufferRequest() = delete;
    DrawArraysInstancedCommandBufferRequest(int mode, int first, int count, int instanceCount)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
        , first(first)
        , count(count)
        , instanceCount(instanceCount)
    {
    }
    DrawArraysInstancedCommandBufferRequest(const DrawArraysInstancedCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
        , first(that.first)
        , count(that.count)
        , instanceCount(that.instanceCount)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << "GL::DrawArraysInstanced(mode=" << mode << ", "
         << first << ", "
         << count << ", "
         << instanceCount << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed instanced draw arrays information.
     * This is crucial for GPU profiling as instanced draw calls can have significant performance impact.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus instanced draw arrays details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add instanced draw arrays specific details
      rapidjson::Value drawDetails(rapidjson::kObjectType);
      drawDetails.AddMember("mode", rapidjson::Value().SetInt(mode), allocator);
      drawDetails.AddMember("first", rapidjson::Value().SetInt(first), allocator);
      drawDetails.AddMember("count", rapidjson::Value().SetInt(count), allocator);
      drawDetails.AddMember("instanceCount", rapidjson::Value().SetInt(instanceCount), allocator);
      drawDetails.AddMember("operation", rapidjson::Value().SetString("drawArraysInstanced", allocator), allocator);
      drawDetails.AddMember("primitiveCount", rapidjson::Value().SetInt(count), allocator);
      drawDetails.AddMember("totalPrimitives", rapidjson::Value().SetInt(count * instanceCount), allocator);
      
      cmdInfo.AddMember("drawDetails", drawDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int mode;
    int first;
    int count;
    int instanceCount;
  };

  class DrawElementsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawElementsCommandBufferRequest,
                                            COMMAND_BUFFER_DRAW_ELEMENTS_REQ>
  {
  public:
    DrawElementsCommandBufferRequest() = delete;
    DrawElementsCommandBufferRequest(int mode, int count, int type, int offset)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
        , count(count)
        , indicesType(type)
        , indicesOffset(offset)
    {
    }
    DrawElementsCommandBufferRequest(const DrawElementsCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
        , count(that.count)
        , indicesType(that.indicesType)
        , indicesOffset(that.indicesOffset)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << "GL::DrawElements("
         << WebGLHelper::WebGLEnumToString(mode) << ", "
         << count << ", "
         << indicesType << ", "
         << indicesOffset << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with detailed draw elements information.
     * This is crucial for GPU profiling as indexed draw calls show different performance characteristics.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus draw elements details
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);
      
      // Add draw elements specific details
      rapidjson::Value drawDetails(rapidjson::kObjectType);
      drawDetails.AddMember("mode", rapidjson::Value().SetInt(mode), allocator);
      drawDetails.AddMember("count", rapidjson::Value().SetInt(count), allocator);
      drawDetails.AddMember("indicesType", rapidjson::Value().SetInt(indicesType), allocator);
      drawDetails.AddMember("indicesOffset", rapidjson::Value().SetInt(indicesOffset), allocator);
      drawDetails.AddMember("operation", rapidjson::Value().SetString("drawElements", allocator), allocator);
      drawDetails.AddMember("primitiveCount", rapidjson::Value().SetInt(count), allocator);
      
      // Add human-readable mode name
      std::string modeName = WebGLHelper::WebGLEnumToString(mode);
      drawDetails.AddMember("modeName", rapidjson::Value().SetString(modeName.c_str(), allocator), allocator);
      
      cmdInfo.AddMember("drawDetails", drawDetails, allocator);
      
      return cmdInfo;
    }

  public:
    int mode;
    int count;
    int indicesType;
    int indicesOffset;
  };

  class DrawElementsInstancedCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawElementsInstancedCommandBufferRequest,
                                            COMMAND_BUFFER_DRAW_ELEMENTS_INSTANCED_REQ>
  {
  public:
    DrawElementsInstancedCommandBufferRequest() = delete;
    DrawElementsInstancedCommandBufferRequest(int mode, int count, int type, int offset, int instanceCount)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
        , count(count)
        , indicesType(type)
        , indicesOffset(offset)
        , instanceCount(instanceCount)
    {
    }
    DrawElementsInstancedCommandBufferRequest(const DrawElementsInstancedCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
        , count(that.count)
        , indicesType(that.indicesType)
        , indicesOffset(that.indicesOffset)
        , instanceCount(that.instanceCount)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << "GL::DrawElementsInstanced("
         << WebGLHelper::WebGLEnumToString(mode) << ", "
         << "count=" << count << ", "
         << "type=" << WebGLHelper::WebGLEnumToString(indicesType) << ", "
         << "offset=" << indicesOffset << ", "
         << "instances=" << instanceCount << ")";
      return ss.str();
    }

  public:
    int mode;
    int count;
    int indicesType;
    int indicesOffset;
    int instanceCount;
  };

  class DrawBuffersCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawBuffersCommandBufferRequest, COMMAND_BUFFER_DRAW_BUFFERS_REQ>
  {
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;

  public:
    DrawBuffersCommandBufferRequest() = delete;
    DrawBuffersCommandBufferRequest(size_t n, const uint32_t *bufs)
        : TrCommandBufferSimpleRequest()
        , n(n)
    {
      for (size_t i = 0; i < n; i++)
        this->bufs[i] = bufs[i];
    }
    DrawBuffersCommandBufferRequest(const DrawBuffersCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , n(that.n)
        , bufs()
    {
      for (size_t i = 0; i < that.n; i++)
        this->bufs[i] = that.bufs[i];
    }

  public:
    size_t n;
    uint32_t bufs[16];
  };

  class DrawRangeElementsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DrawRangeElementsCommandBufferRequest,
                                            COMMAND_BUFFER_DRAW_RANGE_ELEMENTS_REQ>
  {
  public:
    DrawRangeElementsCommandBufferRequest() = delete;
    DrawRangeElementsCommandBufferRequest(int mode, int start, int end, int count, int type, int offset)
        : TrCommandBufferSimpleRequest()
        , mode(mode)
        , start(start)
        , end(end)
        , count(count)
        , indicesType(type)
        , indicesOffset(offset)
    {
    }
    DrawRangeElementsCommandBufferRequest(const DrawRangeElementsCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , mode(that.mode)
        , start(that.start)
        , end(that.end)
        , count(that.count)
        , indicesType(that.indicesType)
        , indicesOffset(that.indicesOffset)
    {
    }

  public:
    int mode;
    int start;
    int end;
    int count;
    int indicesType;
    int indicesOffset;
  };
}
