#pragma once

#include "../shared.hpp"
#include "../base.hpp"

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

  class GetBooleanvCommandBufferResponse : public TrCommandBufferSimpleResponse<GetBooleanvCommandBufferResponse>
  {
  public:
    GetBooleanvCommandBufferResponse(GetBooleanvCommandBufferRequest *req, bool value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_BOOLEANV_RES, req),
          value(value)
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

  class GetIntegervCommandBufferResponse : public TrCommandBufferSimpleResponse<GetIntegervCommandBufferResponse>
  {
  public:
    GetIntegervCommandBufferResponse(GetIntegervCommandBufferRequest *req, int value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_INTEGERV_RES, req),
          value(value)
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

  class GetFloatvCommandBufferResponse : public TrCommandBufferSimpleResponse<GetFloatvCommandBufferResponse>
  {
  public:
    GetFloatvCommandBufferResponse(GetFloatvCommandBufferRequest *req, float value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_FLOATV_RES, req),
          value(value)
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

  class GetStringCommandBufferResponse : public TrCommandBufferSimpleResponse<GetStringCommandBufferResponse>
  {
  public:
    GetStringCommandBufferResponse(GetStringCommandBufferRequest *req, std::string &value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_STRING_RES, req),
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

  class GetShaderPrecisionFormatCommandBufferResponse : public TrCommandBufferSimpleResponse<GetShaderPrecisionFormatCommandBufferResponse>
  {
  public:
    GetShaderPrecisionFormatCommandBufferResponse(GetShaderPrecisionFormatCommandBufferRequest *req, int rangeMin, int rangeMax, int precision)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_RES, req),
          rangeMin(rangeMin),
          rangeMax(rangeMax),
          precision(precision)
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

  class GetErrorCommandBufferResponse : public TrCommandBufferSimpleResponse<GetErrorCommandBufferResponse>
  {
  public:
    GetErrorCommandBufferResponse(GetErrorCommandBufferRequest *req, int error)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_ERROR_RES, req),
          error(error)
    {
    }

  public:
    int error;
  };
}
