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

  enum LinkProgramCommandBufferResponseSegmentType
  {
    SEGMENT_ATTRIB_LOCATION,
    SEGMENT_UNIFORM_LOCATION,
    SEGMENT_UNIFORM_BLOCK
  };

  class LinkProgramCommandBufferResponse : public TrCommandBufferResponse
  {
  public:
    LinkProgramCommandBufferResponse(LinkProgramCommandBufferResponse &that)
        : TrCommandBufferResponse(COMMAND_BUFFER_LINK_PROGRAM_RES, that)
    {
      success = that.success;
      size = that.size;
    }
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
      for (auto &attribLocation : attribLocations)
        addAttribLocationSegment(attribLocation, message);
      for (auto &uniformLocation : uniformLocations)
        addUniformLocationSegment(uniformLocation, message);
      for (auto &uniformBlock : uniformBlocks)
        addUniformBlockSegment(uniformBlock, message);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      for (size_t i = 0; i < message.getSegmentCount(); i++)
      {
        auto segment = message.getSegment(i);
        if (segment == nullptr)
          continue;

        auto segmentChars = segment->toVec<char>();
        char *pSourceData = segmentChars.data();

        LinkProgramCommandBufferResponseSegmentType segmentType;
        memcpy(&segmentType, pSourceData, sizeof(LinkProgramCommandBufferResponseSegmentType));
        pSourceData += sizeof(LinkProgramCommandBufferResponseSegmentType);

        size_t nameSize;
        memcpy(&nameSize, pSourceData, sizeof(size_t));
        pSourceData += sizeof(size_t);

        string name(pSourceData, nameSize);
        pSourceData += nameSize;

        switch (segmentType)
        {
        case SEGMENT_ATTRIB_LOCATION:
        {
          int location;
          memcpy(&location, pSourceData, sizeof(int));
          pSourceData += sizeof(int);
          attribLocations.push_back(AttribLocation(name, location));
          break;
        };
        case SEGMENT_UNIFORM_LOCATION:
        {
          int location;
          memcpy(&location, pSourceData, sizeof(int));
          pSourceData += sizeof(int);
          int size;
          memcpy(&size, pSourceData, sizeof(int));
          pSourceData += sizeof(int);
          uniformLocations.push_back(UniformLocation(name, location, size));
          break;
        };
        case SEGMENT_UNIFORM_BLOCK:
        {
          int index;
          memcpy(&index, pSourceData, sizeof(int));
          pSourceData += sizeof(int);
          uniformBlocks.push_back(UniformBlock(name, index));
          break;
        };
        default:
          break;
        }
      }
    }

  private:
    vector<char> getSegmentBase(LinkProgramCommandBufferResponseSegmentType type,
                                string &name)
    {
      vector<char> base;
      base.insert(base.end(),
                  reinterpret_cast<char *>(&type),
                  reinterpret_cast<char *>(&type) + sizeof(LinkProgramCommandBufferResponseSegmentType));
      size_t nameSize = name.size();
      base.insert(base.end(),
                  reinterpret_cast<char *>(&nameSize),
                  reinterpret_cast<char *>(&nameSize) + sizeof(size_t));
      base.insert(base.end(), name.begin(), name.end());
      return base;
    }

    void addAttribLocationSegment(AttribLocation &attribLocation, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(SEGMENT_ATTRIB_LOCATION, attribLocation.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&attribLocation.location),
                  reinterpret_cast<char *>(&attribLocation.location) + sizeof(attribLocation.location));
      message->addSegment(ipc::TrIpcMessageSegment(base));
    }

    void addUniformLocationSegment(UniformLocation &uniformLocation, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(SEGMENT_UNIFORM_LOCATION, uniformLocation.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&uniformLocation.location),
                  reinterpret_cast<char *>(&uniformLocation.location) + sizeof(uniformLocation.location));
      base.insert(base.end(),
                  reinterpret_cast<char *>(&uniformLocation.size),
                  reinterpret_cast<char *>(&uniformLocation.size) + sizeof(uniformLocation.size));
      message->addSegment(ipc::TrIpcMessageSegment(base));
    }

    void addUniformBlockSegment(UniformBlock &uniformBlock, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(SEGMENT_UNIFORM_BLOCK, uniformBlock.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&uniformBlock.index),
                  reinterpret_cast<char *>(&uniformBlock.index) + sizeof(uniformBlock.index));
      message->addSegment(ipc::TrIpcMessageSegment(base));
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
