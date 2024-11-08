#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  class CreateProgramCommandBufferRequest : public TrCommandBufferSimpleRequest<CreateProgramCommandBufferRequest>
  {
  public:
    CreateProgramCommandBufferRequest(int clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_CREATE_PROGRAM_REQ),
                                                      clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class DeleteProgramCommandBufferRequest : public TrCommandBufferSimpleRequest<DeleteProgramCommandBufferRequest>
  {
  public:
    DeleteProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_DELETE_PROGRAM_REQ),
                                                           clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class LinkProgramCommandBufferRequest : public TrCommandBufferSimpleRequest<LinkProgramCommandBufferRequest>
  {
  public:
    LinkProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_LINK_PROGRAM_REQ),
                                                         clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class ActiveInfo
  {
  public:
    ActiveInfo() = default;
    ActiveInfo(const ActiveInfo &that) = default;
    ActiveInfo(string name, int size, int type) : name(name), size(size), type(type)
    {
    }

  public:
    string name;
    int size;
    int type;
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
    SEGMENT_ACTIVE_ATTRIB,
    SEGMENT_ACTIVE_UNIFORM,
    SEGMENT_ATTRIB_LOCATION,
    SEGMENT_UNIFORM_LOCATION,
    SEGMENT_UNIFORM_BLOCK
  };

  class LinkProgramCommandBufferResponse : public TrCommandBufferSimpleResponse<LinkProgramCommandBufferResponse>
  {
  public:
    LinkProgramCommandBufferResponse(LinkProgramCommandBufferResponse &that) : TrCommandBufferSimpleResponse(that), success(that.success) {}
    LinkProgramCommandBufferResponse(LinkProgramCommandBufferRequest *req, bool success)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_LINK_PROGRAM_RES, req),
          success(success)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      for (auto &activeAttrib : activeAttribs)
        addActiveInfoSegment(activeAttrib, SEGMENT_ACTIVE_ATTRIB, message);
      for (auto &activeUniform : activeUniforms)
        addActiveInfoSegment(activeUniform, SEGMENT_ACTIVE_UNIFORM, message);
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
        case SEGMENT_ACTIVE_ATTRIB:
        {
          activeAttribs.push_back(getActiveInfoFromBuffer(name, &pSourceData));
          break;
        }
        case SEGMENT_ACTIVE_UNIFORM:
        {
          activeUniforms.push_back(getActiveInfoFromBuffer(name, &pSourceData));
          break;
        }
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

    ActiveInfo getActiveInfoFromBuffer(string &name, char **pSrcData)
    {
      char *buf = *pSrcData;
      int size;
      {
        memcpy(&size, buf, sizeof(int));
        buf += sizeof(int);
      }
      int type;
      {
        memcpy(&type, buf, sizeof(int));
        buf += sizeof(int);
      }
      return ActiveInfo(name, size, type);
    }

    void addActiveInfoSegment(ActiveInfo &activeInfo, LinkProgramCommandBufferResponseSegmentType segmentType, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(segmentType, activeInfo.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&activeInfo.size),
                  reinterpret_cast<char *>(&activeInfo.size) + sizeof(activeInfo.size));
      base.insert(base.end(),
                  reinterpret_cast<char *>(&activeInfo.type),
                  reinterpret_cast<char *>(&activeInfo.type) + sizeof(activeInfo.type));
      message->addSegment(new ipc::TrIpcMessageSegment(base));
    }

    void addAttribLocationSegment(AttribLocation &attribLocation, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(SEGMENT_ATTRIB_LOCATION, attribLocation.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&attribLocation.location),
                  reinterpret_cast<char *>(&attribLocation.location) + sizeof(attribLocation.location));
      message->addSegment(new ipc::TrIpcMessageSegment(base));
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
      message->addSegment(new ipc::TrIpcMessageSegment(base));
    }

    void addUniformBlockSegment(UniformBlock &uniformBlock, TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(SEGMENT_UNIFORM_BLOCK, uniformBlock.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&uniformBlock.index),
                  reinterpret_cast<char *>(&uniformBlock.index) + sizeof(uniformBlock.index));
      message->addSegment(new ipc::TrIpcMessageSegment(base));
    }

  public:
    bool success;
    vector<ActiveInfo> activeAttribs;
    vector<ActiveInfo> activeUniforms;
    vector<AttribLocation> attribLocations;
    vector<UniformLocation> uniformLocations;
    vector<UniformBlock> uniformBlocks;
  };

  class UseProgramCommandBufferRequest : public TrCommandBufferSimpleRequest<UseProgramCommandBufferRequest>
  {
  public:
    UseProgramCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_USE_PROGRAM_REQ),
                                                        clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class BindAttribLocationCommandBufferRequest : public TrCommandBufferSimpleRequest<BindAttribLocationCommandBufferRequest>
  {
  public:
    BindAttribLocationCommandBufferRequest(uint32_t program, uint32_t index, const string &name) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_BIND_ATTRIB_LOCATION_REQ),
                                                                                                   program(program),
                                                                                                   attribIndex(index),
                                                                                                   attribName(name)
    {
    }
    BindAttribLocationCommandBufferRequest(BindAttribLocationCommandBufferRequest &that) : TrCommandBufferSimpleRequest(that),
                                                                                             program(that.program),
                                                                                             attribIndex(that.attribIndex)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      message->addStringSegment(attribName);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      attribName = message.getSegment(0)->toString();
    }

  public:
    uint32_t program;
    uint32_t attribIndex;
    string attribName;
  };

  class GetProgramParamCommandBufferRequest : public TrCommandBufferSimpleRequest<GetProgramParamCommandBufferRequest>
  {
  public:
    GetProgramParamCommandBufferRequest(uint32_t clientId, uint32_t pname) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_PROGRAM_PARAM_REQ),
                                                                             clientId(clientId),
                                                                             pname(pname)
    {
    }

  public:
    uint32_t clientId;
    uint32_t pname;
  };

  class GetProgramParamCommandBufferResponse : public TrCommandBufferSimpleResponse<GetProgramParamCommandBufferResponse>
  {
  public:
    GetProgramParamCommandBufferResponse(GetProgramParamCommandBufferResponse &that) : TrCommandBufferSimpleResponse(that), value(that.value)
    {
    }
    GetProgramParamCommandBufferResponse(GetProgramParamCommandBufferRequest *req, int value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES, req),
          value(value)
    {
    }

  public:
    int value;
  };

  class GetProgramInfoLogCommandBufferRequest : public TrCommandBufferSimpleRequest<GetProgramInfoLogCommandBufferRequest>
  {
  public:
    GetProgramInfoLogCommandBufferRequest(uint32_t clientId) : TrCommandBufferSimpleRequest(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_REQ),
                                                               clientId(clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class GetProgramInfoLogCommandBufferResponse : public TrCommandBufferSimpleResponse<GetProgramInfoLogCommandBufferResponse>
  {
  public:
    GetProgramInfoLogCommandBufferResponse(GetProgramInfoLogCommandBufferRequest *req, const string infoLog)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES, req),
          infoLog(infoLog)
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
