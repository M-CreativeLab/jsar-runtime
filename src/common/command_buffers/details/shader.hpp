#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class AttachShaderCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<AttachShaderCommandBufferRequest,
                                            COMMAND_BUFFER_ATTACH_SHADER_REQ>
  {
  public:
    AttachShaderCommandBufferRequest() = delete;
    AttachShaderCommandBufferRequest(uint32_t program, uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , program(program)
        , shader(shader)
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class DetachShaderCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DetachShaderCommandBufferRequest,
                                            COMMAND_BUFFER_DETACH_SHADER_REQ>
  {
  public:
    DetachShaderCommandBufferRequest() = delete;
    DetachShaderCommandBufferRequest(uint32_t program, uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , program(program)
        , shader(shader)
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class CreateShaderCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateShaderCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_SHADER_REQ>
  {
  public:
    CreateShaderCommandBufferRequest() = delete;
    CreateShaderCommandBufferRequest(uint32_t clientId, uint32_t type)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
        , shaderType(type)
    {
    }

  public:
    uint32_t clientId;
    uint32_t shaderType;
  };

  class DeleteShaderCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteShaderCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_SHADER_REQ>
  {
  public:
    DeleteShaderCommandBufferRequest() = delete;
    DeleteShaderCommandBufferRequest(uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class CompileShaderCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CompileShaderCommandBufferRequest,
                                            COMMAND_BUFFER_COMPILE_SHADER_REQ>
  {
  public:
    CompileShaderCommandBufferRequest(uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class ShaderSourceCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ShaderSourceCommandBufferRequest,
                                            COMMAND_BUFFER_SHADER_SOURCE_REQ>
  {
  public:
    ShaderSourceCommandBufferRequest(uint32_t shader, const string &source)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
    {
      sourceSize = source.size();
      sourceStr = reinterpret_cast<char *>(malloc(sourceSize));
      memcpy(sourceStr, source.c_str(), sourceSize);
    }
    ~ShaderSourceCommandBufferRequest()
    {
      if (sourceStr != nullptr)
      {
        free(sourceStr);
        sourceStr = nullptr;
      }
    }

  public:
    inline string source()
    {
      return string(sourceStr, sourceSize);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(sourceStr, sourceSize);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto source = message.getSegment(0)->toString();
      sourceSize = source.size();
      sourceStr = reinterpret_cast<char *>(malloc(sourceSize));
      memcpy(sourceStr, source.c_str(), sourceSize);
    }

  public:
    uint32_t shader;
    char *sourceStr;
    size_t sourceSize;
  };

  class GetShaderSourceCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetShaderSourceCommandBufferRequest,
                                            COMMAND_BUFFER_GET_SHADER_SOURCE_REQ>
  {
  public:
    GetShaderSourceCommandBufferRequest() = delete;
    GetShaderSourceCommandBufferRequest(uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderSourceCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetShaderSourceCommandBufferResponse>
  {
  public:
    GetShaderSourceCommandBufferResponse() = delete;
    GetShaderSourceCommandBufferResponse(GetShaderSourceCommandBufferRequest *req)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_SOURCE_RES, req)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(source);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      source = message.getSegment(0)->toString();
    }

  public:
    string source;
  };

  class GetShaderParamCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetShaderParamCommandBufferRequest,
                                            COMMAND_BUFFER_GET_SHADER_PARAM_REQ>
  {
  public:
    GetShaderParamCommandBufferRequest() = delete;
    GetShaderParamCommandBufferRequest(uint32_t shader, uint32_t pname)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
        , pname(pname)
    {
    }

  public:
    uint32_t shader;
    uint32_t pname;
  };

  class GetShaderParamCommandBufferResponse : public TrCommandBufferSimpleResponse<GetShaderParamCommandBufferResponse>
  {
  public:
    GetShaderParamCommandBufferResponse() = delete;
    GetShaderParamCommandBufferResponse(GetShaderParamCommandBufferRequest *req, int32_t value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_PARAM_RES, req)
        , value(value)
    {
    }

  public:
    int32_t value;
  };

  class GetShaderInfoLogCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetShaderInfoLogCommandBufferRequest,
                                            COMMAND_BUFFER_GET_SHADER_INFO_LOG_REQ>
  {
  public:
    GetShaderInfoLogCommandBufferRequest() = delete;
    GetShaderInfoLogCommandBufferRequest(uint32_t shader)
        : TrCommandBufferSimpleRequest()
        , shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderInfoLogCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetShaderInfoLogCommandBufferResponse>
  {
  public:
    GetShaderInfoLogCommandBufferResponse() = delete;
    GetShaderInfoLogCommandBufferResponse(GetShaderInfoLogCommandBufferRequest *req, const string log)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_INFO_LOG_RES, req)
        , infoLog(log)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(infoLog);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      infoLog = message.getSegment(0)->toString();
    }

  public:
    std::string infoLog;
  };
}
