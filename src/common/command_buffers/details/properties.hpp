#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class GetBooleanvCommandBufferRequest : public TrCommandBufferSimpleRequest<GetBooleanvCommandBufferRequest>
  {
  public:
    GetBooleanvCommandBufferRequest(int pname)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_BOOLEANV_REQ),
          pname(pname)
    {
    }

  public:
    int pname;
  };

  class GetBooleanvCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetBooleanvCommandBufferResponse(GetBooleanvCommandBufferRequest *req, bool value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_BOOLEANV_RES, req),
          value(value)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    bool value;
  };

  class GetIntegervCommandBufferRequest : public TrCommandBufferSimpleRequest<GetIntegervCommandBufferRequest>
  {
  public:
    GetIntegervCommandBufferRequest(int pname)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_INTEGERV_REQ),
          pname(pname)
    {
    }

  public:
    int pname;
  };

  class GetIntegervCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetIntegervCommandBufferResponse(GetIntegervCommandBufferRequest *req, int value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_INTEGERV_RES, req),
          value(value)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int value;
  };

  class GetFloatvCommandBufferRequest : public TrCommandBufferSimpleRequest<GetFloatvCommandBufferRequest>
  {
  public:
    GetFloatvCommandBufferRequest(int pname)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_FLOATV_REQ),
          pname(pname)
    {
    }

  public:
    int pname;
  };

  class GetFloatvCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetFloatvCommandBufferResponse(GetFloatvCommandBufferRequest *req, float value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_FLOATV_RES, req),
          value(value)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    float value;
  };

  class GetStringCommandBufferRequest : public TrCommandBufferSimpleRequest<GetStringCommandBufferRequest>
  {
  public:
    GetStringCommandBufferRequest(int pname)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_STRING_REQ),
          pname(pname)
    {
    }

  public:
    int pname;
  };

  class GetStringCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetStringCommandBufferResponse(GetStringCommandBufferRequest *req, std::string &value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_STRING_RES, req),
          value(value)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(value);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      value = message.getSegment(0)->toString();
    }

  public:
    std::string value;
  };

  class GetShaderPrecisionFormatCommandBufferRequest : public TrCommandBufferSimpleRequest<GetShaderPrecisionFormatCommandBufferRequest>
  {
  public:
    GetShaderPrecisionFormatCommandBufferRequest(int shadertype, int precisiontype)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_REQ),
          shadertype(shadertype),
          precisiontype(precisiontype)
    {
    }

  public:
    int shadertype;
    int precisiontype;
  };

  class GetShaderPrecisionFormatCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetShaderPrecisionFormatCommandBufferResponse(GetShaderPrecisionFormatCommandBufferRequest *req, int rangeMin, int rangeMax, int precision)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_RES, req),
          rangeMin(rangeMin),
          rangeMax(rangeMax),
          precision(precision)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int rangeMin;
    int rangeMax;
    int precision;
  };

  class GetErrorCommandBufferRequest : public TrCommandBufferSimpleRequest<GetErrorCommandBufferRequest>
  {
  public:
    GetErrorCommandBufferRequest()
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_ERROR_REQ)
    {
    }
  };

  class GetErrorCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetErrorCommandBufferResponse(GetErrorCommandBufferRequest *req, int error)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_ERROR_RES, req),
          error(error)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int error;
  };
}
