#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateSamplerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateSamplerCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_SAMPLER_REQ>
  {
  public:
    CreateSamplerCommandBufferRequest() = delete;
    CreateSamplerCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateSamplerCommandBufferRequest(const CreateSamplerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteSamplerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteSamplerCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_SAMPLER_REQ>
  {
  public:
    DeleteSamplerCommandBufferRequest() = delete;
    DeleteSamplerCommandBufferRequest(uint32_t sampler)
        : TrCommandBufferSimpleRequest()
        , sampler(sampler)
    {
    }
    DeleteSamplerCommandBufferRequest(const DeleteSamplerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sampler(that.sampler)
    {
    }

  public:
    uint32_t sampler;
  };

  class BindSamplerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindSamplerCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_SAMPLER_REQ>
  {
  public:
    BindSamplerCommandBufferRequest() = delete;
    BindSamplerCommandBufferRequest(uint32_t unit, uint32_t sampler)
        : TrCommandBufferSimpleRequest()
        , unit(unit)
        , sampler(sampler)
    {
    }
    BindSamplerCommandBufferRequest(const BindSamplerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , unit(that.unit)
        , sampler(that.sampler)
    {
    }

  public:
    uint32_t unit;
    uint32_t sampler;
  };

  class SamplerParameteriCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<SamplerParameteriCommandBufferRequest,
                                            COMMAND_BUFFER_SAMPLER_PARAMETERI_REQ>
  {
  public:
    SamplerParameteriCommandBufferRequest() = delete;
    SamplerParameteriCommandBufferRequest(uint32_t sampler, uint32_t pname, int32_t param)
        : TrCommandBufferSimpleRequest()
        , sampler(sampler)
        , pname(pname)
        , param(param)
    {
    }
    SamplerParameteriCommandBufferRequest(const SamplerParameteriCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sampler(that.sampler)
        , pname(that.pname)
        , param(that.param)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
    int32_t param;
  };

  class SamplerParameterfCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<SamplerParameterfCommandBufferRequest,
                                            COMMAND_BUFFER_SAMPLER_PARAMETERF_REQ>
  {
  public:
    SamplerParameterfCommandBufferRequest() = delete;
    SamplerParameterfCommandBufferRequest(uint32_t sampler, uint32_t pname, float param)
        : TrCommandBufferSimpleRequest()
        , sampler(sampler)
        , pname(pname)
        , param(param)
    {
    }
    SamplerParameterfCommandBufferRequest(const SamplerParameterfCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sampler(that.sampler)
        , pname(that.pname)
        , param(that.param)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
    float param;
  };

  class GetSamplerParameterCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetSamplerParameterCommandBufferRequest,
                                            COMMAND_BUFFER_GET_SAMPLER_PARAMETER_REQ>
  {
  public:
    GetSamplerParameterCommandBufferRequest() = delete;
    GetSamplerParameterCommandBufferRequest(uint32_t sampler, uint32_t pname)
        : TrCommandBufferSimpleRequest()
        , sampler(sampler)
        , pname(pname)
    {
    }
    GetSamplerParameterCommandBufferRequest(const GetSamplerParameterCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sampler(that.sampler)
        , pname(that.pname)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
  };

  class GetSamplerParameterCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetSamplerParameterCommandBufferResponse>
  {
  public:
    GetSamplerParameterCommandBufferResponse(GetSamplerParameterCommandBufferRequest *req, int value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_RES, req)
        , value(value)
    {
    }
    GetSamplerParameterCommandBufferResponse(GetSamplerParameterCommandBufferRequest *req, float value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_RES, req)
        , value(value)
    {
    }
    GetSamplerParameterCommandBufferResponse(const GetSamplerParameterCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , value(that.value)
    {
    }

  public:
    int32_t value;
  };

  class IsSamplerCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<IsSamplerCommandBufferRequest,
                                            COMMAND_BUFFER_IS_SAMPLER_REQ>
  {
  public:
    IsSamplerCommandBufferRequest() = delete;
    IsSamplerCommandBufferRequest(uint32_t sampler)
        : TrCommandBufferSimpleRequest()
        , sampler(sampler)
    {
    }
    IsSamplerCommandBufferRequest(const IsSamplerCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , sampler(that.sampler)
    {
    }

  public:
    uint32_t sampler;
  };

  class IsSamplerCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<IsSamplerCommandBufferResponse>
  {
  public:
    IsSamplerCommandBufferResponse(IsSamplerCommandBufferRequest *req, bool value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_IS_SAMPLER_RES, req)
        , value(value)
    {
    }
    IsSamplerCommandBufferResponse(const IsSamplerCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , value(that.value)
    {
    }

  public:
    bool value;
  };
}
