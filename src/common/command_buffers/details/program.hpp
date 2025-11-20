#pragma once

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  enum LinkProgramCommandBufferResponseSegmentType
  {
    SEGMENT_ACTIVE_ATTRIB,
    SEGMENT_ACTIVE_UNIFORM,
    SEGMENT_ATTRIB_LOCATION,
    SEGMENT_UNIFORM_LOCATION,
    SEGMENT_UNIFORM_BLOCK
  };

  class ActiveInfo
  {
  public:
    ActiveInfo() = default;
    ActiveInfo(const ActiveInfo &that) = default;
    ActiveInfo(string name, int size, int type)
        : name(name)
        , size(size)
        , type(type)
    {
    }

  public:
    string name;
    int size = 0;
    int type = 0;
  };

  class Location
  {
  public:
    Location(string name, int location)
        : name(name)
        , location(location)
    {
    }

  public:
    string name;
    int location;
  };
  using AttribLocation = Location;
  using UniformLocation = Location;

  class UniformBlock
  {
  public:
    UniformBlock(string name, int index)
        : name(name)
        , index(index)
    {
    }

  public:
    string name;
    int index;
  };

  class CreateProgramCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<CreateProgramCommandBufferRequest,
                                            COMMAND_BUFFER_CREATE_PROGRAM_REQ>
  {
  public:
    CreateProgramCommandBufferRequest() = delete;
    CreateProgramCommandBufferRequest(int clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    CreateProgramCommandBufferRequest(const CreateProgramCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }

    /**
     * Serialize the command buffer to a JSON object with program creation information.
     * Program creation is critical for shader pipeline setup and GPU state management.
     * 
     * @param allocator The JSON allocator to use for creating the JSON object
     * @returns A JSON object containing base command info plus program creation parameters
     */
    rapidjson::Value toJson(rapidjson::Document::AllocatorType &allocator) const override
    {
      // Get base command information with new structure
      rapidjson::Value cmdInfo = TrCommandBufferBase::toJson(allocator);

      // Add program creation parameters to the parameters array in OpenGL function order
      rapidjson::Value &parameters = cmdInfo["parameters"];
      parameters.PushBack(rapidjson::Value().SetUint(clientId), allocator);

      return cmdInfo;
    }

  public:
    uint32_t clientId;
  };

  class DeleteProgramCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<DeleteProgramCommandBufferRequest,
                                            COMMAND_BUFFER_DELETE_PROGRAM_REQ>
  {
  public:
    DeleteProgramCommandBufferRequest() = delete;
    DeleteProgramCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    DeleteProgramCommandBufferRequest(const DeleteProgramCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }

  public:
    uint32_t clientId;
  };

  class LinkProgramCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<LinkProgramCommandBufferRequest,
                                            COMMAND_BUFFER_LINK_PROGRAM_REQ>
  {
  public:
    LinkProgramCommandBufferRequest() = delete;
    LinkProgramCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    LinkProgramCommandBufferRequest(const LinkProgramCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      for (auto &attribLocation : attribLocations)
        addLocationSegment(SEGMENT_ATTRIB_LOCATION, attribLocation, message);
      for (auto &uniformLocation : uniformLocations)
        addLocationSegment(SEGMENT_UNIFORM_LOCATION, uniformLocation, message);
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
          uniformLocations.push_back(UniformLocation(name, location));
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

    void addLocationSegment(LinkProgramCommandBufferResponseSegmentType type,
                            Location &loc,
                            TrCommandBufferMessage *message)
    {
      auto base = getSegmentBase(type, loc.name);
      base.insert(base.end(),
                  reinterpret_cast<char *>(&loc.location),
                  reinterpret_cast<char *>(&loc.location) + sizeof(loc.location));
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
    uint32_t clientId;
    vector<AttribLocation> attribLocations;
    vector<UniformLocation> uniformLocations;
    vector<UniformBlock> uniformBlocks;
  };

  class ValidateProgramCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<ValidateProgramCommandBufferRequest,
                                            COMMAND_BUFFER_VALIDATE_PROGRAM_REQ>
  {
  public:
    ValidateProgramCommandBufferRequest() = delete;
    ValidateProgramCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    ValidateProgramCommandBufferRequest(const ValidateProgramCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }

  public:
    uint32_t clientId;
  };

  class UseProgramCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<UseProgramCommandBufferRequest, COMMAND_BUFFER_USE_PROGRAM_REQ>
  {
  public:
    UseProgramCommandBufferRequest() = delete;
    UseProgramCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    UseProgramCommandBufferRequest(const UseProgramCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << clientId << ")";
      return ss.str();
    }

  public:
    uint32_t clientId;
  };

  class BindAttribLocationCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<BindAttribLocationCommandBufferRequest,
                                            COMMAND_BUFFER_BIND_ATTRIB_LOCATION_REQ>
  {
  public:
    BindAttribLocationCommandBufferRequest() = delete;
    BindAttribLocationCommandBufferRequest(uint32_t program, uint32_t index, const string &name)
        : TrCommandBufferSimpleRequest()
        , program(program)
        , attribIndex(index)
        , attribName(name)
    {
    }
    BindAttribLocationCommandBufferRequest(const BindAttribLocationCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , program(that.program)
        , attribIndex(that.attribIndex)
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

  class GetProgramParamCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetProgramParamCommandBufferRequest,
                                            COMMAND_BUFFER_GET_PROGRAM_PARAM_REQ>
  {
  public:
    GetProgramParamCommandBufferRequest() = delete;
    GetProgramParamCommandBufferRequest(uint32_t clientId, uint32_t pname)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
        , pname(pname)
    {
    }
    GetProgramParamCommandBufferRequest(const GetProgramParamCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
        , pname(that.pname)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << clientId << ", "
         << WebGLHelper::WebGLProgramParameterToString(pname) << ")";
      return ss.str();
    }

  public:
    uint32_t clientId;
    uint32_t pname;
  };

  class GetProgramParamCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetProgramParamCommandBufferResponse>
  {
  public:
    GetProgramParamCommandBufferResponse(GetProgramParamCommandBufferRequest *req, int value)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_PROGRAM_PARAM_RES, req)
        , value(value)
    {
    }
    GetProgramParamCommandBufferResponse(const GetProgramParamCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , value(that.value)
    {
    }

  public:
    int value;
  };

  class GetProgramInfoLogCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<GetProgramInfoLogCommandBufferRequest,
                                            COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_REQ>
  {
  public:
    GetProgramInfoLogCommandBufferRequest() = delete;
    GetProgramInfoLogCommandBufferRequest(uint32_t clientId)
        : TrCommandBufferSimpleRequest()
        , clientId(clientId)
    {
    }
    GetProgramInfoLogCommandBufferRequest(const GetProgramInfoLogCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , clientId(that.clientId)
    {
    }

  public:
    uint32_t clientId;
  };

  class GetProgramInfoLogCommandBufferResponse final
      : public TrCommandBufferSimpleResponse<GetProgramInfoLogCommandBufferResponse>
  {
  public:
    GetProgramInfoLogCommandBufferResponse(GetProgramInfoLogCommandBufferRequest *req, const string infoLog)
        : TrCommandBufferSimpleResponse(COMMAND_BUFFER_GET_PROGRAM_INFO_LOG_RES, req)
        , infoLog(infoLog)
    {
    }
    GetProgramInfoLogCommandBufferResponse(const GetProgramInfoLogCommandBufferResponse &that, bool clone = false)
        : TrCommandBufferSimpleResponse(that, clone)
        , infoLog()
    {
      if (clone)
        infoLog = that.infoLog;
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

  class TransformFeedbackVaryingsCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<TransformFeedbackVaryingsCommandBufferRequest,
                                            COMMAND_BUFFER_TRANSFORM_FEEDBACK_VARYINGS_REQ>
  {
  public:
    TransformFeedbackVaryingsCommandBufferRequest() = delete;
    TransformFeedbackVaryingsCommandBufferRequest(uint32_t program, const std::vector<std::string> &varyings, uint32_t bufferMode)
        : TrCommandBufferSimpleRequest()
        , program(program)
        , varyings(varyings)
        , bufferMode(bufferMode)
    {
    }
    TransformFeedbackVaryingsCommandBufferRequest(const TransformFeedbackVaryingsCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , program(that.program)
        , varyings(that.varyings)
        , bufferMode(that.bufferMode)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "(" << program << ", [";
      for (size_t i = 0; i < varyings.size(); i++)
      {
        if (i > 0)
          ss << ", ";
        ss << varyings[i];
      }
      ss << "], " << bufferMode << ")";
      return ss.str();
    }

    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      // Serialize the varyings array
      for (const auto &varying : varyings)
      {
        message->addStringSegment(const_cast<std::string &>(varying));
      }
      return message;
    }

    void deserialize(TrCommandBufferMessage &message) override
    {
      varyings.clear();
      for (size_t i = 0; i < message.getSegmentCount(); i++)
      {
        auto segment = message.getSegment(i);
        if (segment != nullptr)
        {
          varyings.push_back(segment->toString());
        }
      }
    }

  public:
    uint32_t program;
    std::vector<std::string> varyings;
    uint32_t bufferMode;
  };
}
