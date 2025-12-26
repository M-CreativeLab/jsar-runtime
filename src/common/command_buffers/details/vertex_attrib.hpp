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
    VertexAttrib1fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0)
        : VertexAttribNfCommandBufferRequest(program, location_name, v0)
    {
    }
    VertexAttrib1fCommandBufferRequest(uint32_t program, uint32_t index, float v0)
        : VertexAttribNfCommandBufferRequest(program, index, v0)
    {
    }
    VertexAttrib1fCommandBufferRequest(const VertexAttrib1fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , v0(that.v0)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << v0 << ")";
      return ss.str();
    }

    void setFloatValues(float v0_val)
    {
      v0 = v0_val;
    }

  public:
    float v0;
  };

  class VertexAttrib2fCommandBufferRequest final
      : public VertexAttribNfCommandBufferRequest<VertexAttrib2fCommandBufferRequest,
                                                  COMMAND_BUFFER_VERTEX_ATTRIB_2F_REQ,
                                                  float,
                                                  float>
  {
  public:
    VertexAttrib2fCommandBufferRequest() = delete;
    VertexAttrib2fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0, float v1)
        : VertexAttribNfCommandBufferRequest(program, location_name, v0, v1)
    {
    }
    VertexAttrib2fCommandBufferRequest(uint32_t program, uint32_t index, float v0, float v1)
        : VertexAttribNfCommandBufferRequest(program, index, v0, v1)
    {
    }
    VertexAttrib2fCommandBufferRequest(const VertexAttrib2fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << v0 << ", " << v1 << ")";
      return ss.str();
    }

    void setFloatValues(float v0_val, float v1_val)
    {
      v0 = v0_val;
      v1 = v1_val;
    }

  public:
    float v0, v1;
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
    VertexAttrib3fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0, float v1, float v2)
        : VertexAttribNfCommandBufferRequest(program, location_name, v0, v1, v2)
    {
    }
    VertexAttrib3fCommandBufferRequest(uint32_t program, uint32_t index, float v0, float v1, float v2)
        : VertexAttribNfCommandBufferRequest(program, index, v0, v1, v2)
    {
    }
    VertexAttrib3fCommandBufferRequest(const VertexAttrib3fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << v0 << ", " << v1 << ", " << v2 << ")";
      return ss.str();
    }

    void setFloatValues(float v0_val, float v1_val, float v2_val)
    {
      v0 = v0_val;
      v1 = v1_val;
      v2 = v2_val;
    }

  public:
    float v0, v1, v2;
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
    VertexAttrib4fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0, float v1, float v2, float v3)
        : VertexAttribNfCommandBufferRequest(program, location_name, v0, v1, v2, v3)
    {
    }
    VertexAttrib4fCommandBufferRequest(uint32_t program, uint32_t index, float v0, float v1, float v2, float v3)
        : VertexAttribNfCommandBufferRequest(program, index, v0, v1, v2, v3)
    {
    }
    VertexAttrib4fCommandBufferRequest(const VertexAttrib4fCommandBufferRequest &that, bool clone = false)
        : VertexAttribNfCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
        , v3(that.v3)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest::toString(line_prefix) << "("
         << v0 << ", " << v1 << ", " << v2 << ", " << v3 << ")";
      return ss.str();
    }

    void setFloatValues(float v0_val, float v1_val, float v2_val, float v3_val)
    {
      v0 = v0_val;
      v1 = v1_val;
      v2 = v2_val;
      v3 = v3_val;
    }

  public:
    float v0, v1, v2, v3;
  };

  // Base class for vector float vertex attribute requests
  template <typename Derived, CommandBufferType Type>
  class VertexAttribNfvCommandBufferRequest : public SetVertexAttribCommandBufferRequest<Derived, Type>
  {
  public:
    VertexAttribNfvCommandBufferRequest() = delete;
    VertexAttribNfvCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<float> &values)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, location_name)
        , values(values)
    {
    }
    VertexAttribNfvCommandBufferRequest(uint32_t program, uint32_t index, const std::vector<float> &values)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, index)
        , values(values)
    {
    }
    VertexAttribNfvCommandBufferRequest(const Derived &that, bool clone = false)
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
    std::vector<float> values;
  };

  // VertexAttrib1fv command buffer request
  class VertexAttrib1fvCommandBufferRequest final
      : public VertexAttribNfvCommandBufferRequest<VertexAttrib1fvCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_1FV_REQ>
  {
  public:
    using VertexAttribNfvCommandBufferRequest::VertexAttribNfvCommandBufferRequest;
  };

  // VertexAttrib2fv command buffer request
  class VertexAttrib2fvCommandBufferRequest final
      : public VertexAttribNfvCommandBufferRequest<VertexAttrib2fvCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_2FV_REQ>
  {
  public:
    using VertexAttribNfvCommandBufferRequest::VertexAttribNfvCommandBufferRequest;
  };

  // VertexAttrib3fv command buffer request
  class VertexAttrib3fvCommandBufferRequest final
      : public VertexAttribNfvCommandBufferRequest<VertexAttrib3fvCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_3FV_REQ>
  {
  public:
    using VertexAttribNfvCommandBufferRequest::VertexAttribNfvCommandBufferRequest;
  };

  // VertexAttrib4fv command buffer request
  class VertexAttrib4fvCommandBufferRequest final
      : public VertexAttribNfvCommandBufferRequest<VertexAttrib4fvCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_4FV_REQ>
  {
  public:
    using VertexAttribNfvCommandBufferRequest::VertexAttribNfvCommandBufferRequest;
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
  class VertexAttribINiCommandBufferRequest : public SetVertexAttribCommandBufferRequest<Derived, Type>
  {
  public:
    VertexAttribINiCommandBufferRequest() = delete;
    VertexAttribINiCommandBufferRequest(uint32_t program, const std::string &location_name, ValueType v0, ValueType v1, ValueType v2, ValueType v3)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, location_name)
        , v0(v0)
        , v1(v1)
        , v2(v2)
        , v3(v3)
    {
    }
    VertexAttribINiCommandBufferRequest(uint32_t program, uint32_t index, ValueType v0, ValueType v1, ValueType v2, ValueType v3)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(program, index)
        , v0(v0)
        , v1(v1)
        , v2(v2)
        , v3(v3)
    {
    }
    VertexAttribINiCommandBufferRequest(const Derived &that, bool clone = false)
        : SetVertexAttribCommandBufferRequest<Derived, Type>(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
        , v3(that.v3)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << SetVertexAttribCommandBufferRequest<Derived, Type>::toString(line_prefix) << "("
         << v0 << ", " << v1 << ", " << v2 << ", " << v3 << ")";
      return ss.str();
    }

  public:
    ValueType v0, v1, v2, v3;
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
      : public VertexAttribINiCommandBufferRequest<VertexAttribI4iCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_I4I_REQ,
                                                   int>
  {
  public:
    using VertexAttribINiCommandBufferRequest::VertexAttribINiCommandBufferRequest;
  };

  // VertexAttribI4ui command buffer request
  class VertexAttribI4uiCommandBufferRequest final
      : public VertexAttribINiCommandBufferRequest<VertexAttribI4uiCommandBufferRequest,
                                                   COMMAND_BUFFER_VERTEX_ATTRIB_I4UI_REQ,
                                                   uint32_t>
  {
  public:
    using VertexAttribINiCommandBufferRequest::VertexAttribINiCommandBufferRequest;
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
