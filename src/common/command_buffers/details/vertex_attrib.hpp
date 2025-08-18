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
        , componentSize(size)
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
        , componentSize(size)
        , componentType(type)
        , normalized(normalized)
        , stride(stride)
        , offset(offset)
    {
    }
    VertexAttribPointerCommandBufferRequest(const VertexAttribPointerCommandBufferRequest &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest(that, clone)
        , componentSize(that.componentSize)
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
         << componentSize << ", "
         << componentType << ", "
         << normalized << ", "
         << stride << ", "
         << offset << ")";
      return ss.str();
    }

  public:
    uint32_t componentSize;
    uint32_t componentType;
    uint32_t normalized;
    uint32_t stride;
    uint32_t offset;
  };

  // Base class template for VertexAttribNf commands (1f, 2f, 3f, 4f)
  template <typename Derived, CommandBufferType Type, typename... FloatArgs>
  class VertexAttribNfCommandBufferRequest : public SetVertexAttribCommandBufferRequest<Derived, Type>
  {
  public:
    VertexAttribNfCommandBufferRequest() = delete;
    VertexAttribNfCommandBufferRequest(uint32_t program, const std::string &location_name, FloatArgs... args)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, location_name)
    {
      setValues(args...);
    }
    VertexAttribNfCommandBufferRequest(uint32_t program, uint32_t index, FloatArgs... args)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, index)
    {
      setValues(args...);
    }
    VertexAttribNfCommandBufferRequest(const Derived &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(that, clone)
    {
    }

  protected:
    template <typename... Args>
    void setValues(Args... args)
    {
      static_cast<Derived *>(this)->setFloatValues(args...);
    }
  };

  class VertexAttrib1fCommandBufferRequest final
      : public VertexAttribNfCommandBufferRequest<VertexAttrib1fCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_1F_REQ,
                                                  float>
  {
  public:
    VertexAttrib1fCommandBufferRequest() = delete;
    VertexAttrib1fCommandBufferRequest(uint32_t program, const std::string &location_name, float x)
        : VertexAttribNfCommandBufferRequest(program, location_name, x)
    {
    }
    VertexAttrib1fCommandBufferRequest(uint32_t program, uint32_t index, float x)
        : VertexAttribNfCommandBufferRequest(program, index, x)
    {
    }
    VertexAttrib1fCommandBufferRequest(const VertexAttrib1fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
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

    void setFloatValues(float x_val)
    {
      x = x_val;
    }

  public:
    float x;
  };

  class VertexAttrib2fCommandBufferRequest final
      : public VertexAttribNfCommandBufferRequest<VertexAttrib2fCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_2F_REQ,
                                                  float,
                                                  float>
  {
  public:
    VertexAttrib2fCommandBufferRequest() = delete;
    VertexAttrib2fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y)
        : VertexAttribNfCommandBufferRequest(program, location_name, x, y)
    {
    }
    VertexAttrib2fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y)
        : VertexAttribNfCommandBufferRequest(program, index, x, y)
    {
    }
    VertexAttrib2fCommandBufferRequest(const VertexAttrib2fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , x(that.x)
        , y(that.y)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ")";
      return ss.str();
    }

    void setFloatValues(float x_val, float y_val)
    {
      x = x_val;
      y = y_val;
    }

  public:
    float x, y;
  };

  class VertexAttrib3fCommandBufferRequest final
      : public VertexAttribNfCommandBufferRequest<VertexAttrib3fCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_3F_REQ,
                                                  float,
                                                  float,
                                                  float>
  {
  public:
    VertexAttrib3fCommandBufferRequest() = delete;
    VertexAttrib3fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y, float z)
        : VertexAttribNfCommandBufferRequest(program, location_name, x, y, z)
    {
    }
    VertexAttrib3fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y, float z)
        : VertexAttribNfCommandBufferRequest(program, index, x, y, z)
    {
    }
    VertexAttrib3fCommandBufferRequest(const VertexAttrib3fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , x(that.x)
        , y(that.y)
        , z(that.z)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ", " << z << ")";
      return ss.str();
    }

    void setFloatValues(float x_val, float y_val, float z_val)
    {
      x = x_val;
      y = y_val;
      z = z_val;
    }

  public:
    float x, y, z;
  };

  class VertexAttrib4fCommandBufferRequest final
      : public VertexAttribNfCommandBufferRequest<VertexAttrib4fCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_4F_REQ,
                                                  float,
                                                  float,
                                                  float,
                                                  float>
  {
  public:
    VertexAttrib4fCommandBufferRequest() = delete;
    VertexAttrib4fCommandBufferRequest(uint32_t program, const std::string &location_name, float x, float y, float z, float w)
        : VertexAttribNfCommandBufferRequest(program, location_name, x, y, z, w)
    {
    }
    VertexAttrib4fCommandBufferRequest(uint32_t program, uint32_t index, float x, float y, float z, float w)
        : VertexAttribNfCommandBufferRequest(program, index, x, y, z, w)
    {
    }
    VertexAttrib4fCommandBufferRequest(const VertexAttrib4fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , x(that.x)
        , y(that.y)
        , z(that.z)
        , w(that.w)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << x << ", " << y << ", " << z << ", " << w << ")";
      return ss.str();
    }

    void setFloatValues(float x_val, float y_val, float z_val, float w_val)
    {
      x = x_val;
      y = y_val;
      z = z_val;
      w = w_val;
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

  // Base class template for VertexAttribI4 commands (I4i, I4ui)
  template <typename Derived, CommandBufferType Type, typename ValueType>
  class VertexAttribI4CommandBufferRequest : public SetVertexAttribCommandBufferRequest<Derived, Type>
  {
  public:
    VertexAttribI4CommandBufferRequest() = delete;
    VertexAttribI4CommandBufferRequest(uint32_t program, const std::string &location_name, ValueType x, ValueType y, ValueType z, ValueType w)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, location_name)
        , x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }
    VertexAttribI4CommandBufferRequest(uint32_t program, uint32_t index, ValueType x, ValueType y, ValueType z, ValueType w)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, index)
        , x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }
    VertexAttribI4CommandBufferRequest(const Derived &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(that, clone)
        , x(that.x)
        , y(that.y)
        , z(that.z)
        , w(that.w)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest<Derived, Type>::toString(line_prefix) << "("
         << x << ", " << y << ", " << z << ", " << w << ")";
      return ss.str();
    }

  public:
    ValueType x, y, z, w;
  };

  // Base class template for VertexAttribI4v commands (I4iv, I4uiv)
  template <typename Derived, CommandBufferType Type, typename ValueType>
  class VertexAttribI4vCommandBufferRequest : public SetVertexAttribCommandBufferRequest<Derived, Type>
  {
  public:
    VertexAttribI4vCommandBufferRequest() = delete;
    VertexAttribI4vCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<ValueType> &values)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, location_name)
        , values(values)
    {
    }
    VertexAttribI4vCommandBufferRequest(uint32_t program, uint32_t index, const std::vector<ValueType> &values)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, index)
        , values(values)
    {
    }
    VertexAttribI4vCommandBufferRequest(const Derived &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(that, clone)
        , values(that.values)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest<Derived, Type>::toString(line_prefix) << "([";
      for (size_t i = 0; i < values.size(); ++i)
      {
        if (i > 0)
          ss << ", ";
        ss << values[i];
      }
      ss << "])";
      return ss.str();
    }

  public:
    std::vector<ValueType> values;
  };

  // VertexAttribI4i command buffer request
  class VertexAttribI4iCommandBufferRequest final
      : public VertexAttribI4CommandBufferRequest<VertexAttribI4iCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_I4I_REQ,
                                                  int>
  {
  public:
    using VertexAttribI4CommandBufferRequest::VertexAttribI4CommandBufferRequest;
  };

  // VertexAttribI4ui command buffer request
  class VertexAttribI4uiCommandBufferRequest final
      : public VertexAttribI4CommandBufferRequest<VertexAttribI4uiCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_I4UI_REQ,
                                                  uint32_t>
  {
  public:
    using VertexAttribI4CommandBufferRequest::VertexAttribI4CommandBufferRequest;
  };

  // VertexAttribI4iv command buffer request
  class VertexAttribI4ivCommandBufferRequest final
      : public VertexAttribI4vCommandBufferRequest<VertexAttribI4ivCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_I4IV_REQ,
                                                   int>
  {
  public:
    using VertexAttribI4vCommandBufferRequest::VertexAttribI4vCommandBufferRequest;
  };

  // VertexAttribI4uiv command buffer request
  class VertexAttribI4uivCommandBufferRequest final
      : public VertexAttribI4vCommandBufferRequest<VertexAttribI4uivCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_I4UIV_REQ,
                                                   uint32_t>
  {
  public:
    using VertexAttribI4vCommandBufferRequest::VertexAttribI4vCommandBufferRequest;
  };
}
