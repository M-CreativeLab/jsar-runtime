#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateSamplerCommandBufferRequest,
                                                                                COMMAND_BUFFER_CREATE_SAMPLER_REQ>
  {
  public:
    CreateSamplerCommandBufferRequest() = delete;
    CreateSamplerCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(),
                                                           clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteSamplerCommandBufferRequest,
                                                                                COMMAND_BUFFER_DELETE_SAMPLER_REQ>
  {
  public:
    DeleteSamplerCommandBufferRequest() = delete;
    DeleteSamplerCommandBufferRequest(uint32_t sampler) : TrCommandBufferSimpleRequest(),
                                                          sampler(sampler)
    {
    }

  public:
    uint32_t sampler;
  };

  class BindSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<BindSamplerCommandBufferRequest,
                                                                              COMMAND_BUFFER_BIND_SAMPLER_REQ>
  {
  public:
    BindSamplerCommandBufferRequest() = delete;
    BindSamplerCommandBufferRequest(uint32_t unit, uint32_t sampler)
        : TrCommandBufferSimpleRequest(),
          unit(unit),
          sampler(sampler)
    {
    }

  public:
    uint32_t unit;
    uint32_t sampler;
  };

  class SamplerParameteriCommandBufferRequest : public TrCommandBufferSimpleRequest<SamplerParameteriCommandBufferRequest,
                                                                                    COMMAND_BUFFER_SAMPLER_PARAMETERI_REQ>
  {
  public:
    SamplerParameteriCommandBufferRequest() = delete;
    SamplerParameteriCommandBufferRequest(uint32_t sampler, uint32_t pname, int32_t param)
        : TrCommandBufferSimpleRequest(),
          sampler(sampler),
          pname(pname),
          param(param)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
    int32_t param;
  };

  class SamplerParameterfCommandBufferRequest : public TrCommandBufferSimpleRequest<SamplerParameterfCommandBufferRequest,
                                                                                    COMMAND_BUFFER_SAMPLER_PARAMETERF_REQ>
  {
  public:
    SamplerParameterfCommandBufferRequest() = delete;
    SamplerParameterfCommandBufferRequest(uint32_t sampler, uint32_t pname, float param)
        : TrCommandBufferSimpleRequest(),
          sampler(sampler),
          pname(pname),
          param(param)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
    float param;
  };

  class GetSamplerParameterCommandBufferRequest : public TrCommandBufferSimpleRequest<GetSamplerParameterCommandBufferRequest,
                                                                                      COMMAND_BUFFER_GET_SAMPLER_PARAMETER_REQ>
  {
  public:
    GetSamplerParameterCommandBufferRequest() = delete;
    GetSamplerParameterCommandBufferRequest(uint32_t sampler, uint32_t pname)
        : TrCommandBufferSimpleRequest(),
          sampler(sampler),
          pname(pname)
    {
    }

  public:
    uint32_t sampler;
    uint32_t pname;
  };

  class GetSamplerParameterCommandBufferResponse : public TrCommandBufferSimpleResponse<GetSamplerParameterCommandBufferResponse>
  {
  public:
    GetSamplerParameterCommandBufferResponse(GetSamplerParameterCommandBufferRequest *req, int value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_RES, req),
          value(value)
    {
    }

  public:
    int32_t value;
  };

  class IsSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<IsSamplerCommandBufferRequest,
                                                                            COMMAND_BUFFER_IS_SAMPLER_REQ>
  {
  public:
    IsSamplerCommandBufferRequest() = delete;
    IsSamplerCommandBufferRequest(uint32_t sampler)
        : TrCommandBufferSimpleRequest(),
          sampler(sampler)
    {
    }

  public:
    uint32_t sampler;
  };

  class IsSamplerCommandBufferResponse : public TrCommandBufferSimpleResponse<IsSamplerCommandBufferResponse>
  {
  public:
    IsSamplerCommandBufferResponse(IsSamplerCommandBufferRequest *req, bool value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_IS_SAMPLER_RES, req),
          value(value)
    {
    }

  public:
    bool value;
  };
}
