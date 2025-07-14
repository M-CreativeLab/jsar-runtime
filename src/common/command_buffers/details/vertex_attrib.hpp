#pragma once

#include <sstream>

#include "../shared.hpp"
#include "../base.hpp"

namespace commandbuffers
{
  // The base class for all uniform command buffer requests which stores the program and location information.
  class SetVertexAttribCommandBufferRequestBase
  {
  public:
    SetVertexAttribCommandBufferRequestBase(uint32_t program, const std::string &location_name)
        : program(program)
        , location(0)
        , locationAvailable(false)
        , locationQueryName(location_name)
    {
    }
    // Create a vertex attrib command buffer request with a program and location index directly.
    SetVertexAttribCommandBufferRequestBase(uint32_t program, uint32_t loc)
        : program(program)
        , location(loc)
        , locationAvailable(true)
        , locationQueryName("")
    {
    }
    SetVertexAttribCommandBufferRequestBase(const SetVertexAttribCommandBufferRequestBase &that, bool clone = false)
        : program(that.program)
        , location(that.location)
        , locationAvailable(that.locationAvailable)
    {
      if (clone)
        locationQueryName = that.locationQueryName;
    }

  public:
    void setLoc(std::optional<uint32_t> index, bool clear_name = false)
    {
      if (index.has_value())
      {
        location = index.value_or(0);
        locationAvailable = true;
        if (clear_name)
          locationQueryName.clear();
      }
    }
    std::string locToString() const
    {
      std::stringstream loc_ss;
      loc_ss << "Loc(";
      {
        if (locationAvailable)
          loc_ss << location;
        else
          loc_ss << "?";
      }

      if (!locationQueryName.empty())
        loc_ss << ", " << locationQueryName;
      loc_ss << ")";
      return loc_ss.str();
    }

  public:
    uint32_t program;
    uint32_t location;
    bool locationAvailable;
    std::string locationQueryName;
  };

  template <typename Derived, CommandBufferType Type>
  class SetVertexAttribCommandBufferRequest : public TrCommandBufferSimpleRequest<Derived, Type>,
                                              public SetVertexAttribCommandBufferRequestBase
  {
  public:
    SetVertexAttribCommandBufferRequest(uint32_t program, const std::string &location_name)
        : TrCommandBufferSimpleRequest<Derived, Type>()
        , SetVertexAttribCommandBufferRequestBase(program, location_name)
    {
    }
    SetVertexAttribCommandBufferRequest(uint32_t program, uint32_t loc)
        : TrCommandBufferSimpleRequest<Derived, Type>()
        , SetVertexAttribCommandBufferRequestBase(program, loc)
    {
    }
    SetVertexAttribCommandBufferRequest(const Derived &that, bool clone = false)
        : TrCommandBufferSimpleRequest<Derived, Type>(that, clone)
        , SetVertexAttribCommandBufferRequestBase(that, clone)
    {
    }

  public:
    virtual TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(this->type, this->size, this);
      if (!this->locationQueryName.empty())
        message->addStringSegment(this->locationQueryName);
      return message;
    }
    virtual void deserialize(TrCommandBufferMessage &message) override
    {
      auto nameSegment = message.nextSegment();
      if (nameSegment != nullptr)
        this->locationQueryName = nameSegment->toString();
    }
    virtual std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest<Derived, Type>::toString(line_prefix)
         << "(" << locToString() << ")";
      return ss.str();
    }
  };

  class EnableVertexAttribArrayCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<EnableVertexAttribArrayCommandBufferRequest,
                                                   COMMAND_BUFFER_ENABLE_VERTEX_ATTRIB_ARRAY_REQ>
  {
    using SetVertexAttribCommandBufferRequest::SetVertexAttribCommandBufferRequest;
  };

  class DisableVertexAttribArrayCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<DisableVertexAttribArrayCommandBufferRequest,
                                                   COMMAND_BUFFER_DISABLE_VERTEX_ATTRIB_ARRAY_REQ>
  {
    using SetVertexAttribCommandBufferRequest::SetVertexAttribCommandBufferRequest;
  };

  class VertexAttribPointerCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttribPointerCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_POINTER_REQ>
  {
  public:
    VertexAttribPointerCommandBufferRequest() = delete;
    VertexAttribPointerCommandBufferRequest(uint32_t program,
                                            const std::string &location_name,
                                            uint32_t size,
                                            uint32_t type,
                                            uint32_t normalized,
                                            uint32_t stride,
                                            uint32_t offset)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , conponentSize(size)
        , componentType(type)
        , normalized(normalized)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribPointerCommandBufferRequest(uint32_t program,
                                            uint32_t index,
                                            uint32_t size,
                                            uint32_t type,
                                            uint32_t normalized,
                                            uint32_t stride,
                                            uint32_t offset)
        : SetVertexAttribCommandBufferRequest(program, index)
        , conponentSize(size)
        , componentType(type)
        , normalized(normalized)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribPointerCommandBufferRequest(const VertexAttribPointerCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , conponentSize(that.conponentSize)
        , componentType(that.componentType)
        , normalized(that.normalized)
        , stride(that.stride)
        , offset(that.offset)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", "
         << conponentSize << ", "
         << componentType << ", "
         << normalized << ", "
         << stride << ", "
         << offset << ")";
      return ss.str();
    }

  public:
    uint32_t conponentSize;
    uint32_t componentType;
    uint32_t normalized;
    uint32_t stride;
    uint32_t offset;
  };

  class VertexAttribIPointerCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttribIPointerCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_IPOINTER_REQ>
  {
  public:
    VertexAttribIPointerCommandBufferRequest() = delete;
    VertexAttribIPointerCommandBufferRequest(uint32_t program,
                                             const std::string &location_name,
                                             uint32_t size,
                                             uint32_t type,
                                             uint32_t stride,
                                             uint32_t offset)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , componentSize(size)
        , componentType(type)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribIPointerCommandBufferRequest(uint32_t program,
                                             uint32_t index,
                                             uint32_t size,
                                             uint32_t type,
                                             uint32_t stride,
                                             uint32_t offset)
        : SetVertexAttribCommandBufferRequest(program, index)
        , componentSize(size)
        , componentType(type)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribIPointerCommandBufferRequest(const VertexAttribIPointerCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , componentSize(that.componentSize)
        , componentType(that.componentType)
        , stride(that.stride)
        , offset(that.offset)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", "
         << componentSize << ", "
         << componentType << ", " << stride << ", " << offset << ")";
      return ss.str();
    }

  public:
    uint32_t componentSize;
    uint32_t componentType;
    uint32_t stride;
    uint32_t offset;
  };

  class VertexAttribDivisorCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttribDivisorCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_DIVISOR_REQ>
  {
  public:
    VertexAttribDivisorCommandBufferRequest() = delete;
    VertexAttribDivisorCommandBufferRequest(uint32_t program, const std::string &location_name, uint32_t divisor)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , divisor(divisor)
    {
    }
    VertexAttribDivisorCommandBufferRequest(uint32_t program, uint32_t index, uint32_t divisor)
        : SetVertexAttribCommandBufferRequest(program, index)
        , divisor(divisor)
    {
    }
    VertexAttribDivisorCommandBufferRequest(const VertexAttribDivisorCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , divisor(that.divisor)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", "
         << divisor << ")";
      return ss.str();
    }

  public:
    uint32_t divisor;
  };
}
