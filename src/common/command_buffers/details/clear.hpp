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

  // Base class for ClearBuffer commands that use vector values
  template <typename Derived, CommandBufferType Type, typename ValueType>
  class ClearBufferBaseRequest : public TrCommandBufferSimpleRequest<Derived, Type>
  {
  public:
    ClearBufferBaseRequest() = delete;
    ClearBufferBaseRequest(int buffer, int drawbuffer, const std::vector<ValueType> &values)
        : TrCommandBufferSimpleRequest<Derived, Type>()
        , buffer(buffer)
        , drawbuffer(drawbuffer)
        , values(values)
    {
    }
    ClearBufferBaseRequest(const ClearBufferBaseRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest<Derived, Type>(that, clone)
        , buffer(that.buffer)
        , drawbuffer(that.drawbuffer)
        , values(that.values)
    {
    }

    TrCommandBufferMessage *serialize() override
    {
      auto message = TrCommandBufferSimpleRequest<Derived, Type>::serialize();
      if (values.size() > 0)
        message->addVecSegment(values);
      return message;
    }

    void deserialize(TrCommandBufferMessage &message) override
    {
      TrCommandBufferSimpleRequest<Derived, Type>::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<ValueType>();
      else
        values.clear();
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest<Derived, Type>::toString(line_prefix) << "("
         << buffer << ", " << drawbuffer << ", [" << values.size() << " values])";
      return ss.str();
    }

  public:
    int buffer;
    int drawbuffer;
    std::vector<ValueType> values;
  };

  class ClearBufferfvCommandBufferRequest final
      : public ClearBufferBaseRequest<ClearBufferfvCommandBufferRequest,
                                      COMMAND_BUFFER_CLEAR_BUFFERFV_REQ,
                                      float>
  {
  public:
    ClearBufferfvCommandBufferRequest() = delete;
    ClearBufferfvCommandBufferRequest(int buffer, int drawbuffer, const std::vector<float> &values)
        : ClearBufferBaseRequest(buffer, drawbuffer, values)
    {
    }
    ClearBufferfvCommandBufferRequest(const ClearBufferfvCommandBufferRequest &that, bool clone = false)
        : ClearBufferBaseRequest(that, clone)
    {
    }
  };

  class ClearBufferivCommandBufferRequest final
      : public ClearBufferBaseRequest<ClearBufferivCommandBufferRequest,
                                      COMMAND_BUFFER_CLEAR_BUFFERIV_REQ,
                                      int>
  {
  public:
    ClearBufferivCommandBufferRequest() = delete;
    ClearBufferivCommandBufferRequest(int buffer, int drawbuffer, const std::vector<int> &values)
        : ClearBufferBaseRequest(buffer, drawbuffer, values)
    {
    }
    ClearBufferivCommandBufferRequest(const ClearBufferivCommandBufferRequest &that, bool clone = false)
        : ClearBufferBaseRequest(that, clone)
    {
    }
  };

  class ClearBufferuivCommandBufferRequest final
      : public ClearBufferBaseRequest<ClearBufferuivCommandBufferRequest,
                                      COMMAND_BUFFER_CLEAR_BUFFERUIV_REQ,
                                      unsigned int>
  {
  public:
    ClearBufferuivCommandBufferRequest() = delete;
    ClearBufferuivCommandBufferRequest(int buffer, int drawbuffer, const std::vector<unsigned int> &values)
        : ClearBufferBaseRequest(buffer, drawbuffer, values)
    {
    }
    ClearBufferuivCommandBufferRequest(const ClearBufferuivCommandBufferRequest &that, bool clone = false)
        : ClearBufferBaseRequest(that, clone)
    {
    }
  };

  class ClearBufferfiCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ClearBufferfiCommandBufferRequest,
                                            COMMAND_BUFFER_CLEAR_BUFFERFI_REQ>
  {
  public:
    ClearBufferfiCommandBufferRequest() = delete;
    ClearBufferfiCommandBufferRequest(int buffer, int drawbuffer, float depth, int stencil)
        : TrCommandBufferSimpleRequest()
        , buffer(buffer)
        , drawbuffer(drawbuffer)
        , depth(depth)
        , stencil(stencil)
    {
    }
    ClearBufferfiCommandBufferRequest(const ClearBufferfiCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , buffer(that.buffer)
        , drawbuffer(that.drawbuffer)
        , depth(that.depth)
        , stencil(that.stencil)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << buffer << ", " << drawbuffer << ", " << depth << ", " << stencil << ")";
      return ss.str();
    }

  public:
    int buffer;
    int drawbuffer;
    float depth;
    int stencil;
  };
}
