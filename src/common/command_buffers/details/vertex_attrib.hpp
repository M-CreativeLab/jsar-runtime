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

  class VertexAttrib1fCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttrib1fCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_1F_REQ>
  {
  public:
    VertexAttrib1fCommandBufferRequest() = delete;
    VertexAttrib1fCommandBufferRequest(uint32_t program, const std::string &location_name, float x)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , x(x)
    {
    }
    VertexAttrib1fCommandBufferRequest(uint32_t program, uint32_t index, float x)
        : SetVertexAttribCommandBufferRequest(program, index)
        , x(x)
    {
    }
    VertexAttrib1fCommandBufferRequest(const VertexAttrib1fCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , x(that.x)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ")";
      return ss.str();
    }

  public:
    float x;
  };

  class VertexAttrib2fCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttrib2fCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_2F_REQ>
  {
  public:
    VertexAttrib2fCommandBufferRequest() = delete;
    VertexAttrib2fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , x(x), y(y)
    {
    }
    VertexAttrib2fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y)
        : SetVertexAttribCommandBufferRequest(program, index)
        , x(x), y(y)
    {
    }
    VertexAttrib2fCommandBufferRequest(const VertexAttrib2fCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , x(that.x), y(that.y)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ")";
      return ss.str();
    }

  public:
    float x, y;
  };

  class VertexAttrib3fCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttrib3fCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_3F_REQ>
  {
  public:
    VertexAttrib3fCommandBufferRequest() = delete;
    VertexAttrib3fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y, float z)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , x(x), y(y), z(z)
    {
    }
    VertexAttrib3fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y, float z)
        : SetVertexAttribCommandBufferRequest(program, index)
        , x(x), y(y), z(z)
    {
    }
    VertexAttrib3fCommandBufferRequest(const VertexAttrib3fCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , x(that.x), y(that.y), z(that.z)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ", " << z << ")";
      return ss.str();
    }

  public:
    float x, y, z;
  };

  class VertexAttrib4fCommandBufferRequest final
      : public SetVertexAttribCommandBufferRequest<VertexAttrib4fCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_4F_REQ>
  {
  public:
    VertexAttrib4fCommandBufferRequest() = delete;
    VertexAttrib4fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y, float z, float w)
        : SetVertexAttribCommandBufferRequest(program, location_name)
        , x(x), y(y), z(z), w(w)
    {
    }
    VertexAttrib4fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y, float z, float w)
        : SetVertexAttribCommandBufferRequest(program, index)
        , x(x), y(y), z(z), w(w)
    {
    }
    VertexAttrib4fCommandBufferRequest(const VertexAttrib4fCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , x(that.x), y(that.y), z(that.z), w(that.w)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ", " << z << ", " << w << ")";
      return ss.str();
    }

  public:
    float x, y, z, w;
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

// VertexAttribI4i command buffer request
class VertexAttribI4iCommandBufferRequest final
    : public SetVertexAttribCommandBufferRequest<VertexAttribI4iCommandBufferRequest,
                                                 COMMAND_BUFFER_VERTEX_ATTRIB_I4I_REQ>
{
public:
  VertexAttribI4iCommandBufferRequest() = delete;
  VertexAttribI4iCommandBufferRequest(uint32_t program, const std::string &location_name, int x, int y, int z, int w)
      : SetVertexAttribCommandBufferRequest(program, location_name)
      , x(x), y(y), z(z), w(w)
  {
  }
  VertexAttribI4iCommandBufferRequest(uint32_t program, uint32_t index, int x, int y, int z, int w)
      : SetVertexAttribCommandBufferRequest(program, index)
      , x(x), y(y), z(z), w(w)
  {
  }
  VertexAttribI4iCommandBufferRequest(const VertexAttribI4iCommandBufferRequest &that, bool clone = false)
      : SetVertexAttribCommandBufferRequest(that, clone)
      , x(that.x), y(that.y), z(that.z), w(that.w)
  {
  }

  std::string toString(const char *line_prefix) const override
  {
    std::stringstream ss;
    ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
       << x << ", " << y << ", " << z << ", " << w << ")";
    return ss.str();
  }

public:
  int x, y, z, w;
};

// VertexAttribI4ui command buffer request
class VertexAttribI4uiCommandBufferRequest final
    : public SetVertexAttribCommandBufferRequest<VertexAttribI4uiCommandBufferRequest,
                                                 COMMAND_BUFFER_VERTEX_ATTRIB_I4UI_REQ>
{
public:
  VertexAttribI4uiCommandBufferRequest() = delete;
  VertexAttribI4uiCommandBufferRequest(uint32_t program, const std::string &location_name, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
      : SetVertexAttribCommandBufferRequest(program, location_name)
      , x(x), y(y), z(z), w(w)
  {
  }
  VertexAttribI4uiCommandBufferRequest(uint32_t program, uint32_t index, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
      : SetVertexAttribCommandBufferRequest(program, index)
      , x(x), y(y), z(z), w(w)
  {
  }
  VertexAttribI4uiCommandBufferRequest(const VertexAttribI4uiCommandBufferRequest &that, bool clone = false)
      : SetVertexAttribCommandBufferRequest(that, clone)
      , x(that.x), y(that.y), z(that.z), w(that.w)
  {
  }

  std::string toString(const char *line_prefix) const override
  {
    std::stringstream ss;
    ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
       << x << ", " << y << ", " << z << ", " << w << ")";
    return ss.str();
  }

public:
  uint32_t x, y, z, w;
};

// VertexAttribI4iv command buffer request
class VertexAttribI4ivCommandBufferRequest final
    : public SetVertexAttribCommandBufferRequest<VertexAttribI4ivCommandBufferRequest,
                                                 COMMAND_BUFFER_VERTEX_ATTRIB_I4IV_REQ>
{
public:
  VertexAttribI4ivCommandBufferRequest() = delete;
  VertexAttribI4ivCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<int> &values)
      : SetVertexAttribCommandBufferRequest(program, location_name)
      , values(values)
  {
  }
  VertexAttribI4ivCommandBufferRequest(uint32_t program, uint32_t index, const std::vector<int> &values)
      : SetVertexAttribCommandBufferRequest(program, index)
      , values(values)
  {
  }
  VertexAttribI4ivCommandBufferRequest(const VertexAttribI4ivCommandBufferRequest &that, bool clone = false)
      : SetVertexAttribCommandBufferRequest(that, clone)
      , values(that.values)
  {
  }

  std::string toString(const char *line_prefix) const override
  {
    std::stringstream ss;
    ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "([";
    for (size_t i = 0; i < values.size(); ++i) {
      if (i > 0) ss << ", ";
      ss << values[i];
    }
    ss << "])";
    return ss.str();
  }

public:
  std::vector<int> values;
};

// VertexAttribI4uiv command buffer request
class VertexAttribI4uivCommandBufferRequest final
    : public SetVertexAttribCommandBufferRequest<VertexAttribI4uivCommandBufferRequest,
                                                 COMMAND_BUFFER_VERTEX_ATTRIB_I4UIV_REQ>
{
public:
  VertexAttribI4uivCommandBufferRequest() = delete;
  VertexAttribI4uivCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<uint32_t> &values)
      : SetVertexAttribCommandBufferRequest(program, location_name)
      , values(values)
  {
  }
  VertexAttribI4uivCommandBufferRequest(uint32_t program, uint32_t index, const std::vector<uint32_t> &values)
      : SetVertexAttribCommandBufferRequest(program, index)
      , values(values)
  {
  }
  VertexAttribI4uivCommandBufferRequest(const VertexAttribI4uivCommandBufferRequest &that, bool clone = false)
      : SetVertexAttribCommandBufferRequest(that, clone)
      , values(that.values)
  {
  }

  std::string toString(const char *line_prefix) const override
  {
    std::stringstream ss;
    ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "([";
    for (size_t i = 0; i < values.size(); ++i) {
      if (i > 0) ss << ", ";
      ss << values[i];
    }
    ss << "])";
    return ss.str();
  }

public:
  std::vector<uint32_t> values;
};
}
