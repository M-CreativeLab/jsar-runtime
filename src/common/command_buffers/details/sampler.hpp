#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateSamplerCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateSamplerCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_CREATE_SAMPLER_REQ),
                                                           clientId(clientId)
    {
      size = sizeof(CreateSamplerCommandBufferRequest);
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
    uint32_t clientId;
  };

  class DeleteSamplerCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteSamplerCommandBufferRequest(uint32_t sampler) : TrCommandBufferBase(COMMAND_BUFFER_DELETE_SAMPLER_REQ),
                                                          sampler(sampler)
    {
      size = sizeof(DeleteSamplerCommandBufferRequest);
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
    uint32_t sampler;
  };

  class BindSamplerCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    BindSamplerCommandBufferRequest(uint32_t unit, uint32_t sampler)
        : TrCommandBufferBase(COMMAND_BUFFER_BIND_SAMPLER_REQ),
          unit(unit),
          sampler(sampler)
    {
      size = sizeof(BindSamplerCommandBufferRequest);
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
    uint32_t unit;
    uint32_t sampler;
  };

  class SamplerParameteriCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    SamplerParameteriCommandBufferRequest(uint32_t sampler, uint32_t pname, int32_t param)
        : TrCommandBufferBase(COMMAND_BUFFER_SAMPLER_PARAMETERI_REQ),
          sampler(sampler),
          pname(pname),
          param(param)
    {
      size = sizeof(SamplerParameteriCommandBufferRequest);
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
    uint32_t sampler;
    uint32_t pname;
    int32_t param;
  };

  class SamplerParameterfCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    SamplerParameterfCommandBufferRequest(uint32_t sampler, uint32_t pname, float param)
        : TrCommandBufferBase(COMMAND_BUFFER_SAMPLER_PARAMETERF_REQ),
          sampler(sampler),
          pname(pname),
          param(param)
    {
      size = sizeof(SamplerParameterfCommandBufferRequest);
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
    uint32_t sampler;
    uint32_t pname;
    float param;
  };

  class GetSamplerParameterCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetSamplerParameterCommandBufferRequest(uint32_t sampler, uint32_t pname)
        : TrCommandBufferBase(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_REQ),
          sampler(sampler),
          pname(pname)
    {
      size = sizeof(GetSamplerParameterCommandBufferRequest);
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
    uint32_t sampler;
    uint32_t pname;
  };

  class GetSamplerParameterCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetSamplerParameterCommandBufferResponse(GetSamplerParameterCommandBufferRequest *req, int value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_SAMPLER_PARAMETER_RES, req),
          value(value)
    {
      size = sizeof(GetSamplerParameterCommandBufferResponse);
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
    int32_t value;
  };

  class IsSamplerCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    IsSamplerCommandBufferRequest(uint32_t sampler)
        : TrCommandBufferBase(COMMAND_BUFFER_IS_SAMPLER_REQ),
          sampler(sampler)
    {
      size = sizeof(IsSamplerCommandBufferRequest);
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
    uint32_t sampler;
  };

  class IsSamplerCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    IsSamplerCommandBufferResponse(IsSamplerCommandBufferRequest *req, bool value)
        : TrCommandBufferResponse(COMMAND_BUFFER_IS_SAMPLER_RES, req),
          value(value)
    {
      size = sizeof(IsSamplerCommandBufferResponse);
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
}
