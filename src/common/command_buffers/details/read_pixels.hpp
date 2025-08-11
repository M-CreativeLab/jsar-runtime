#pragma once

#include "../base.hpp"
#include "../shared.hpp"
#include "../webgl_constants.hpp"

namespace commandbuffers
{
  class ReadPixelsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ReadPixelsCommandBufferRequest, COMMAND_BUFFER_READ_PIXELS_REQ>
  {
  public:
    ReadPixelsCommandBufferRequest() = delete;
    ReadPixelsCommandBufferRequest(int x, int y, int width, int height, uint32_t format, uint32_t type)
        : TrCommandBufferSimpleRequest()
        , x(x)
        , y(y)
        , width(width)
        , height(height)
        , format(format)
        , type(type)
    {
    }
    ReadPixelsCommandBufferRequest(const ReadPixelsCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , x(that.x)
        , y(that.y)
        , width(that.width)
        , height(that.height)
        , format(that.format)
        , type(that.type)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << x << ", "
         << y << ", "
         << width << ", "
         << height << ", "
         << WebGLHelper::WebGLEnumToString(format) << ", "
         << WebGLHelper::WebGLEnumToString(type) << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with read pixels information.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus read pixels parameters
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information with new structure
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);

      // Add read pixels parameters to the parameters array in OpenGL function order
      rapidjson::Value &parameters = cmdInfo["parameters"];
      parameters.PushBack(rapidjson::Value().SetInt(x), allocator);
      parameters.PushBack(rapidjson::Value().SetInt(y), allocator);
      parameters.PushBack(rapidjson::Value().SetInt(width), allocator);
      parameters.PushBack(rapidjson::Value().SetInt(height), allocator);
      parameters.PushBack(rapidjson::Value().SetUint(format), allocator);
      parameters.PushBack(rapidjson::Value().SetUint(type), allocator);

      return cmdInfo;
    }

  public:
    int x;
    int y;
    int width;
    int height;
    uint32_t format;
    uint32_t type;
  };

  class ReadPixelsCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<ReadPixelsCommandBufferResponse>
  {
  public:
    ReadPixelsCommandBufferResponse(ReadPixelsCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_READ_PIXELS_RES, req)
        , pixelData(nullptr)
        , pixelDataSize(0)
    {
    }
    ReadPixelsCommandBufferResponse(const ReadPixelsCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , pixelData(nullptr)
        , pixelDataSize(0)
    {
      if (clone && that.pixelData != nullptr && that.pixelDataSize > 0)
      {
        pixelDataSize = that.pixelDataSize;
        pixelData = malloc(pixelDataSize);
        if (pixelData != nullptr)
          memcpy(pixelData, that.pixelData, pixelDataSize);
      }
    }
    ~ReadPixelsCommandBufferResponse()
    {
      if (pixelData != nullptr)
      {
        free(pixelData);
        pixelData = nullptr;
      }
    }

    void setPixelData(void *data, size_t size)
    {
      if (pixelData != nullptr)
      {
        free(pixelData);
        pixelData = nullptr;
      }
      
      if (data != nullptr && size > 0)
      {
        pixelDataSize = size;
        pixelData = malloc(size);
        if (pixelData != nullptr)
          memcpy(pixelData, data, size);
      }
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (pixelData != nullptr && pixelDataSize > 0)
        message->addRawSegment(pixelDataSize, pixelData);
      return message;
    }
    
    void deserialize(TrCommandBufferMessage &message) override
    {
      if (message.getSegmentCount() > 0)
      {
        auto dataSegment = message.getSegment(0);
        if (dataSegment != nullptr)
        {
          pixelDataSize = dataSegment->getSize();
          if (pixelDataSize > 0)
          {
            pixelData = malloc(pixelDataSize);
            if (pixelData != nullptr)
              memcpy(pixelData, dataSegment->getData(), pixelDataSize);
          }
        }
      }
    }

  public:
    void *pixelData;
    size_t pixelDataSize;
  };
}