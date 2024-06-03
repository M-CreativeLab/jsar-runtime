#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class AttachShaderCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    AttachShaderCommandBufferRequest(uint32_t program, uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_ATTACH_SHADER_REQ),
                                                                          program(program),
                                                                          shader(shader)
    {
      size = sizeof(AttachShaderCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class DetachShaderCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DetachShaderCommandBufferRequest(uint32_t program, uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_DETACH_SHADER_REQ),
                                                                          program(program),
                                                                          shader(shader)
    {
      size = sizeof(DetachShaderCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t program;
    uint32_t shader;
  };

  class CreateShaderCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateShaderCommandBufferRequest(uint32_t clientId, uint32_t type)
        : TrCommandBufferBase(COMMAND_BUFFER_CREATE_SHADER_REQ),
          clientId(clientId),
          shaderType(type)
    {
      size = sizeof(CreateShaderCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t clientId;
    uint32_t shaderType;
  };

  class DeleteShaderCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteShaderCommandBufferRequest(uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_DELETE_SHADER_REQ),
                                                        shader(shader)
    {
      size = sizeof(DeleteShaderCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t shader;
  };

  class CompileShaderCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CompileShaderCommandBufferRequest(uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_COMPILE_SHADER_REQ),
                                                         shader(shader)
    {
      size = sizeof(CompileShaderCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t shader;
  };

  class ShaderSourceCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    ShaderSourceCommandBufferRequest(uint32_t shader, const string &source) : TrCommandBufferBase(COMMAND_BUFFER_SHADER_SOURCE_REQ),
                                                                              shader(shader)
    {
      size = sizeof(ShaderSourceCommandBufferRequest);
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
    string source()
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

  class GetShaderSourceCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetShaderSourceCommandBufferRequest(uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_GET_SHADER_SOURCE_REQ),
                                                           shader(shader)
    {
      size = sizeof(GetShaderSourceCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderSourceCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetShaderSourceCommandBufferResponse(GetShaderSourceCommandBufferRequest *req)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_SHADER_SOURCE_RES, req)
    {
      size = sizeof(GetShaderSourceCommandBufferResponse);
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

  class GetShaderParamCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetShaderParamCommandBufferRequest(uint32_t shader, uint32_t pname) : TrCommandBufferBase(COMMAND_BUFFER_GET_SHADER_PARAM_REQ),
                                                                          shader(shader),
                                                                          pname(pname)
    {
      size = sizeof(GetShaderParamCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t shader;
    uint32_t pname;
  };

  class GetShaderParamCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetShaderParamCommandBufferResponse(GetShaderParamCommandBufferRequest *req, int32_t value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_SHADER_PARAM_RES, req), value(value)
    {
      size = sizeof(GetShaderParamCommandBufferResponse);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    int32_t value;
  };

  class GetShaderInfoLogCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetShaderInfoLogCommandBufferRequest(uint32_t shader) : TrCommandBufferBase(COMMAND_BUFFER_GET_SHADER_INFO_LOG_REQ),
                                                            shader(shader)
    {
      size = sizeof(GetShaderInfoLogCommandBufferRequest);
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      return new TrCommandBufferMessage(type, size, this);
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
    }

  public:
    uint32_t shader;
  };

  class GetShaderInfoLogCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetShaderInfoLogCommandBufferResponse(GetShaderInfoLogCommandBufferRequest *req, const string &log)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_SHADER_INFO_LOG_RES, req), infoLog(log)
    {
      size = sizeof(GetShaderInfoLogCommandBufferResponse);
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
