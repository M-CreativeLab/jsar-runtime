#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class AttachShaderCommandBufferRequest : public TrCommandBufferSimpleRequest<AttachShaderCommandBufferRequest>
  {
  public:
    AttachShaderCommandBufferRequest(uint32_t program, uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_ATTACH_SHADER_REQ),
                                                                          program(program),
                                                                          shader(shader)
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class DetachShaderCommandBufferRequest : public TrCommandBufferSimpleRequest<DetachShaderCommandBufferRequest>
  {
  public:
    DetachShaderCommandBufferRequest(uint32_t program, uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DETACH_SHADER_REQ),
                                                                          program(program),
                                                                          shader(shader)
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class CreateShaderCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateShaderCommandBufferRequest>
  {
  public:
    CreateShaderCommandBufferRequest(uint32_t clientId, uint32_t type)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_SHADER_REQ),
          clientId(clientId),
          shaderType(type)
    {
    }

  public:
    uint32_t clientId;
    uint32_t shaderType;
  };

  class DeleteShaderCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteShaderCommandBufferRequest>
  {
  public:
    DeleteShaderCommandBufferRequest(uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_SHADER_REQ),
                                                        shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class CompileShaderCommandBufferRequest : public TrCommandBufferSimpleRequest<CompileShaderCommandBufferRequest>
  {
  public:
    CompileShaderCommandBufferRequest(uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_COMPILE_SHADER_REQ),
                                                         shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class ShaderSourceCommandBufferRequest : public TrCommandBufferSimpleRequest<ShaderSourceCommandBufferRequest>
  {
  public:
    ShaderSourceCommandBufferRequest(uint32_t shader, const string &source) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_SHADER_SOURCE_REQ),
                                                                              shader(shader)
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

  class GetShaderSourceCommandBufferRequest : public TrCommandBufferSimpleRequest<GetShaderSourceCommandBufferRequest>
  {
  public:
    GetShaderSourceCommandBufferRequest(uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_SHADER_SOURCE_REQ),
                                                           shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderSourceCommandBufferResponse : public TrCommandBufferSimpleResponse<GetShaderSourceCommandBufferResponse>
  {
  public:
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

  class GetShaderParamCommandBufferRequest : public TrCommandBufferSimpleRequest<GetShaderParamCommandBufferRequest>
  {
  public:
    GetShaderParamCommandBufferRequest(uint32_t shader, uint32_t pname) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_SHADER_PARAM_REQ),
                                                                          shader(shader),
                                                                          pname(pname)
    {
    }

  public:
    uint32_t shader;
    uint32_t pname;
  };

  class GetShaderParamCommandBufferResponse : public TrCommandBufferSimpleResponse<GetShaderParamCommandBufferResponse>
  {
  public:
    GetShaderParamCommandBufferResponse(GetShaderParamCommandBufferRequest *req, int32_t value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_PARAM_RES, req), value(value)
    {
    }

  public:
    int32_t value;
  };

  class GetShaderInfoLogCommandBufferRequest : public TrCommandBufferSimpleRequest<GetShaderInfoLogCommandBufferRequest>
  {
  public:
    GetShaderInfoLogCommandBufferRequest(uint32_t shader) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_SHADER_INFO_LOG_REQ),
                                                            shader(shader)
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderInfoLogCommandBufferResponse : public TrCommandBufferSimpleResponse<GetShaderInfoLogCommandBufferResponse>
  {
  public:
    GetShaderInfoLogCommandBufferResponse(GetShaderInfoLogCommandBufferRequest *req, const string log)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_SHADER_INFO_LOG_RES, req), infoLog(log)
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
    string infoLog;
  };
}
