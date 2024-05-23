#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../message.hpp"

namespace commandbuffers
{
  class CreateProgramCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    CreateProgramCommandBufferRequest(int clientId) : TrCommandBufferBase(COMMAND_BUFFER_CREATE_PROGRAM_REQ),
                                                      clientId(clientId)
    {
      size = sizeof(CreateProgramCommandBufferRequest);
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
  };

  class DeleteProgramCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    DeleteProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_DELETE_PROGRAM_REQ),
                                                           clientId(clientId)
    {
      size = sizeof(DeleteProgramCommandBufferRequest);
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
  };

  class LinkProgramCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    LinkProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_LINK_PROGRAM_REQ),
                                                         clientId(clientId)
    {
      size = sizeof(LinkProgramCommandBufferRequest);
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
  };

  class AttribLocation
  {
  public:
    AttribLocation(string name, int location) : name(name), location(location)
    {
    }

  public:
    string name;
    int location;
  };

  class UniformLocation
  {
  public:
    UniformLocation(string name, int location, int size)
        : name(name), location(location), size(size)
    {
    }

  public:
    string name;
    int location;
    int size;
  };

  class UniformBlock
  {
  public:
    UniformBlock(string name, int index)
        : name(name), index(index)
    {
    }

  public:
    string name;
    int index;
  };

  class LinkProgramCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    LinkProgramCommandBufferResponse(LinkProgramCommandBufferRequest *req, bool success)
        : TrCommandBufferResponse(COMMAND_BUFFER_LINK_PROGRAM_RES, req),
          success(success)
    {
      size = sizeof(LinkProgramCommandBufferResponse);
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
    bool success;
    vector<AttribLocation> attribLocations;
    vector<UniformLocation> uniformLocations;
    vector<UniformBlock> uniformBlocks;
  };

  class UseProgramCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    UseProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_USE_PROGRAM_REQ),
                                                        clientId(clientId)
    {
      size = sizeof(UseProgramCommandBufferRequest);
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
  };

  class GetProgramParamCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetProgramParamCommandBufferRequest(uint32_t clientId, uint32_t pname) : TrCommandBufferBase(COMMAND_BUFFER_GET_PROGRAM_PARAM_REQ),
                                                                             clientId(clientId),
                                                                             pname(pname)
    {
      size = sizeof(GetProgramParamCommandBufferRequest);
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
    uint32_t pname;
  };

  class GetProgramParamCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetProgramParamCommandBufferResponse(GetProgramParamCommandBufferRequest *req, int value)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES, req),
          value(value)
    {
      size = sizeof(GetProgramParamCommandBufferResponse);
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

  class GetProgramInfoLogCommandBufferRequest : public TrCommandBufferBase
  {
  public:
    GetProgramInfoLogCommandBufferRequest(uint32_t clientId) : TrCommandBufferBase(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_REQ),
                                                               clientId(clientId)
    {
      size = sizeof(GetProgramInfoLogCommandBufferRequest);
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
  };

  class GetProgramInfoLogCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    GetProgramInfoLogCommandBufferResponse(GetProgramInfoLogCommandBufferRequest *req, const string &infoLog)
        : TrCommandBufferResponse(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES, req),
          infoLog(infoLog)
    {
      size = sizeof(GetProgramInfoLogCommandBufferResponse);
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
