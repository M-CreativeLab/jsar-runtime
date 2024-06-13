#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateSamplerCommandBufferRequest>
  {
  public:
    CreateSamplerCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_SAMPLER_REQ),
                                                           clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteSamplerCommandBufferRequest>
  {
  public:
    DeleteSamplerCommandBufferRequest(uint32_t sampler) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_SAMPLER_REQ),
                                                          sampler(sampler)
    {
    }

  public:
    uint32_t sampler;
  };

  class BindSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<BindSamplerCommandBufferRequest>
  {
  public:
    BindSamplerCommandBufferRequest(uint32_t unit, uint32_t sampler)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_SAMPLER_REQ),
          unit(unit),
          sampler(sampler)
    {
    }

  public:
    uint32_t unit;
    uint32_t sampler;
  };

  class SamplerParameteriCommandBufferRequest : public TrCommandBufferSimpleRequest<SamplerParameteriCommandBufferRequest>
  {
  public:
    SamplerParameteriCommandBufferRequest(uint32_t sampler, uint32_t pname, int32_t param)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_SAMPLER_PARAMETERI_REQ),
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

  class SamplerParameterfCommandBufferRequest : public TrCommandBufferSimpleRequest<SamplerParameterfCommandBufferRequest>
  {
  public:
    SamplerParameterfCommandBufferRequest(uint32_t sampler, uint32_t pname, float param)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_SAMPLER_PARAMETERF_REQ),
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

  class GetSamplerParameterCommandBufferRequest : public TrCommandBufferSimpleRequest<GetSamplerParameterCommandBufferRequest>
  {
  public:
    GetSamplerParameterCommandBufferRequest(uint32_t sampler, uint32_t pname)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_REQ),
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

  class IsSamplerCommandBufferRequest : public TrCommandBufferSimpleRequest<IsSamplerCommandBufferRequest>
  {
  public:
    IsSamplerCommandBufferRequest(uint32_t sampler)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_IS_SAMPLER_REQ),
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
