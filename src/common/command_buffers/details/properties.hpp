#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  template <typename Derived, CommandBufferType Type>
  class GetParameterCommandBufferRequest : public TrCommandBufferSimpleRequest<Derived, Type>
  {
  public:
    GetParameterCommandBufferRequest() = delete;
    GetParameterCommandBufferRequest(int pname)
        : TrCommandBufferSimpleRequest<Derived, Type>(),
          pname(pname)
    {
    }

  public:
    int pname;
  };

  template <typename Derived, typename ReqType, typename ValueType, CommandBufferType Type>
  class GetParameterCommandBufferResponse : public TrCommandBufferSimpleResponse<Derived>
  {
  public:
    GetParameterCommandBufferResponse() = delete;
    GetParameterCommandBufferResponse(ReqType *req, ValueType value)
        : TrCommandBufferSimpleResponse<Derived>(Type, req),
          value(value)
    {
    }

  public:
    ValueType value;
  };

  class GetBooleanvCommandBufferRequest final
      : public GetParameterCommandBufferRequest<GetBooleanvCommandBufferRequest,
                                                COMMAND_BUFFER_GET_BOOLEANV_REQ>
  {
  public:
    using GetParameterCommandBufferRequest::GetParameterCommandBufferRequest;
  };

  class GetBooleanvCommandBufferResponse final
      : public GetParameterCommandBufferResponse<GetBooleanvCommandBufferResponse,
                                                 GetBooleanvCommandBufferRequest,
                                                 bool,
                                                 COMMAND_BUFFER_GET_BOOLEANV_RES>
  {
  public:
    using GetParameterCommandBufferResponse::GetParameterCommandBufferResponse;
  };

  class GetIntegervCommandBufferRequest final
      : public GetParameterCommandBufferRequest<GetIntegervCommandBufferRequest,
                                                COMMAND_BUFFER_GET_INTEGERV_REQ>
  {
  public:
    using GetParameterCommandBufferRequest::GetParameterCommandBufferRequest;
  };

  class GetIntegervCommandBufferResponse final
      : public GetParameterCommandBufferResponse<GetIntegervCommandBufferResponse,
                                                 GetIntegervCommandBufferRequest,
                                                 int,
                                                 COMMAND_BUFFER_GET_INTEGERV_RES>
  {
  public:
    using GetParameterCommandBufferResponse::GetParameterCommandBufferResponse;
  };

  class GetFloatvCommandBufferRequest final
      : public GetParameterCommandBufferRequest<GetFloatvCommandBufferRequest,
                                                COMMAND_BUFFER_GET_FLOATV_REQ>
  {
  public:
    using GetParameterCommandBufferRequest::GetParameterCommandBufferRequest;
  };

  class GetFloatvCommandBufferResponse final
      : public GetParameterCommandBufferResponse<GetFloatvCommandBufferResponse,
                                                 GetFloatvCommandBufferRequest,
                                                 float,
                                                 COMMAND_BUFFER_GET_FLOATV_RES>
  {
  public:
    using GetParameterCommandBufferResponse::GetParameterCommandBufferResponse;
  };

  class GetStringCommandBufferRequest final
      : public GetParameterCommandBufferRequest<GetStringCommandBufferRequest,
                                                COMMAND_BUFFER_GET_STRING_REQ>
  {
  public:
    using GetParameterCommandBufferRequest::GetParameterCommandBufferRequest;
  };

  class GetStringCommandBufferResponse final
      : public GetParameterCommandBufferResponse<GetStringCommandBufferResponse,
                                                 GetStringCommandBufferRequest,
                                                 std::string,
                                                 COMMAND_BUFFER_GET_STRING_RES>
  {
  public:
    using GetParameterCommandBufferResponse::GetParameterCommandBufferResponse;

  public:
    GetStringCommandBufferResponse(GetStringCommandBufferResponse &that)
        : GetParameterCommandBufferResponse(that)
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
  };

  class GetShaderPrecisionFormatCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetShaderPrecisionFormatCommandBufferRequest,
                                            COMMAND_BUFFER_GET_SHADER_PRECISION_FORMAT_REQ>
  {
  public:
    GetShaderPrecisionFormatCommandBufferRequest() = delete;
    GetShaderPrecisionFormatCommandBufferRequest(int shadertype, int precisiontype)
        : TrCommandBufferSimpleRequest(),
          shadertype(shadertype),
          precisiontype(precisiontype)
    {
    }

  public:
    int shadertype;
    int precisiontype;
  };

  class GetShaderPrecisionFormatCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetShaderPrecisionFormatCommandBufferResponse>
  {
  public:
    GetShaderPrecisionFormatCommandBufferResponse() = delete;
    GetShaderPrecisionFormatCommandBufferResponse(GetShaderPrecisionFormatCommandBufferRequest *req,
                                                  int rangeMin, int rangeMax, int precision)
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

  class GetErrorCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetErrorCommandBufferRequest, COMMAND_BUFFER_GET_ERROR_REQ>
  {
  public:
    using TrCommandBufferSimpleRequest::TrCommandBufferSimpleRequest;
  };

  class GetErrorCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetErrorCommandBufferResponse>
  {
  public:
    GetErrorCommandBufferResponse() = delete;
    GetErrorCommandBufferResponse(GetErrorCommandBufferRequest *req, int error)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_ERROR_RES, req),
          error(error)
    {
    }

  public:
    int error;
  };
}
