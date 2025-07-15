#pragma once

#include "../shared.hpp"
#include "../base.hpp"
#include "../webgl_placeholders.hpp"

namespace commandbuffers
{
  enum class MatrixHandedness
  {
    MATRIX_LEFT_HANDED = 0,
    MATRIX_RIGHT_HANDED = 1,
  };

  class MatrixComputationGraph
  {
  public:
    MatrixComputationGraph()
        : handedness(MatrixHandedness::MATRIX_LEFT_HANDED)
        , placeholderId(WebGLMatrixPlaceholderId::NotSet)
        , inverseMatrix(false)
        , multiview(false)
    {
    }
    MatrixComputationGraph(WebGLMatrixPlaceholderId placeholder, MatrixHandedness handedness = MatrixHandedness::MATRIX_LEFT_HANDED)
        : placeholderId(placeholder)
        , handedness(handedness)
        , inverseMatrix(false)
        , multiview(false)
    {
    }

  public:
    MatrixHandedness handedness;
    WebGLMatrixPlaceholderId placeholderId;
    bool inverseMatrix;
    /**
     * If true, this computation graph's output is for multiview, namely an array of matrices.
     */
    bool multiview;
  };

  class UniformBlockBindingCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<UniformBlockBindingCommandBufferRequest,
                                            COMMAND_BUFFER_UNIFORM_BLOCK_BINDING_REQ>
  {
  public:
    UniformBlockBindingCommandBufferRequest() = delete;
    UniformBlockBindingCommandBufferRequest(uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding)
        : TrCommandBufferSimpleRequest()
        , program(program)
        , uniformBlockIndex(uniformBlockIndex)
        , uniformBlockBinding(uniformBlockBinding)
    {
    }
    UniformBlockBindingCommandBufferRequest(const UniformBlockBindingCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , program(that.program)
        , uniformBlockIndex(that.uniformBlockIndex)
        , uniformBlockBinding(that.uniformBlockBinding)
    {
    }

  public:
    uint32_t program;
    uint32_t uniformBlockIndex;
    uint32_t uniformBlockBinding;
  };

  // The base class for all uniform command buffer requests which stores the program and location information.
  class SetUniformCommandBufferRequestBase
  {
  public:
    SetUniformCommandBufferRequestBase(uint32_t program, const std::string &location_name)
        : program(program)
        , location(0)
        , locationAvailable(false)
        , locationQueryName(location_name)
    {
    }
    SetUniformCommandBufferRequestBase(const SetUniformCommandBufferRequestBase &that, bool clone = false)
        : program(that.program)
        , location(that.location)
        , locationAvailable(that.locationAvailable)
    {
      if (clone)
        locationQueryName = that.locationQueryName;
    }

  public:
    void setLocationIndex(std::optional<uint32_t> loc_index, bool clear_name = false)
    {
      if (loc_index.has_value())
      {
        location = loc_index.value_or(0);
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
          loc_ss << location << ", ";
        else
          loc_ss << "?" << ", ";
      }
      loc_ss << locationQueryName
             << ")";
      return loc_ss.str();
    }

  public:
    uint32_t program;
    uint32_t location;
    bool locationAvailable;
    std::string locationQueryName;
  };

  template <typename Derived, CommandBufferType Type>
  class SetUniformCommandBufferRequest : public TrCommandBufferSimpleRequest<Derived, Type>,
                                         public SetUniformCommandBufferRequestBase
  {
  public:
    SetUniformCommandBufferRequest(uint32_t program, const std::string &location_name)
        : TrCommandBufferSimpleRequest<Derived, Type>()
        , SetUniformCommandBufferRequestBase(program, location_name)
    {
    }
    SetUniformCommandBufferRequest(const Derived &that, bool clone = false)
        : TrCommandBufferSimpleRequest<Derived, Type>(that, clone)
        , SetUniformCommandBufferRequestBase(that, clone)
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

  class Uniform1fCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform1fCommandBufferRequest, COMMAND_BUFFER_UNIFORM1F_REQ>
  {
  public:
    Uniform1fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
    {
    }
    Uniform1fCommandBufferRequest(const Uniform1fCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ","
         << v0 << ")";
      return ss.str();
    }

  public:
    float v0;
  };

  class Uniform1fvCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform1fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM1FV_REQ>
  {
  public:
    Uniform1fvCommandBufferRequest() = delete;
    Uniform1fvCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<float> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform1fvCommandBufferRequest(const Uniform1fvCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      if (values.size() > 0)
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ","
         << "[";

      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<float> values;
  };

  class Uniform1iCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform1iCommandBufferRequest, COMMAND_BUFFER_UNIFORM1I_REQ>
  {
  public:
    Uniform1iCommandBufferRequest() = delete;
    Uniform1iCommandBufferRequest(uint32_t program, const std::string &location_name, int v0)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
    {
    }
    Uniform1iCommandBufferRequest(const Uniform1iCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ","
         << v0 << ")";
      return os.str();
    }

  public:
    int v0;
  };

  class Uniform1ivCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform1ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM1IV_REQ>
  {
  public:
    Uniform1ivCommandBufferRequest() = delete;
    Uniform1ivCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<int> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform1ivCommandBufferRequest(const Uniform1ivCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      if (values.size() > 0)
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", ";

      os << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<int> values;
  };

  class Uniform2fCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform2fCommandBufferRequest, COMMAND_BUFFER_UNIFORM2F_REQ>
  {
  public:
    Uniform2fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0, float v1)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
        , v1(v1)
    {
    }
    Uniform2fCommandBufferRequest(const Uniform2fCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ","
         << v0 << ","
         << v1 << ")";
      return ss.str();
    }

  public:
    float v0;
    float v1;
  };

  class Uniform2fvCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform2fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM2FV_REQ>
  {
  public:
    Uniform2fvCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<float> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform2fvCommandBufferRequest(const Uniform2fvCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      if (values.size() > 1 && values.size() % 2 == 0) // Check the value size is 2x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", ";

      os << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<float> values;
  };

  class Uniform2iCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform2iCommandBufferRequest, COMMAND_BUFFER_UNIFORM2I_REQ>
  {
  public:
    Uniform2iCommandBufferRequest(uint32_t program, const std::string &location_name, int v0, int v1)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
        , v1(v1)
    {
    }
    Uniform2iCommandBufferRequest(const Uniform2iCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", "
         << v0 << ", "
         << v1 << ")";
      return ss.str();
    }

  public:
    int v0;
    int v1;
  };

  class Uniform2ivCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform2ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM2IV_REQ>
  {
  public:
    Uniform2ivCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<int> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform2ivCommandBufferRequest(const Uniform2ivCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      assert(message != nullptr && "SetUniformCommandBufferRequest::serialize() must not return nullptr");

      if (values.size() > 1 && values.size() % 2 == 0) // Check the value size is 2x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", ";

      ss << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        ss << values[i];
        if (i < values.size() - 1)
          ss << ", ";
      }
      ss << "])";
      return ss.str();
    }

  public:
    std::vector<int> values;
  };

  class Uniform3fCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform3fCommandBufferRequest, COMMAND_BUFFER_UNIFORM3F_REQ>
  {
  public:
    Uniform3fCommandBufferRequest() = delete;
    Uniform3fCommandBufferRequest(uint32_t program, const std::string &location_name, float v0, float v1, float v2)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
        , v1(v1)
        , v2(v2)
    {
    }
    Uniform3fCommandBufferRequest(const Uniform3fCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),"
         << v0 << ","
         << v1 << ","
         << v2 << ")";
      return ss.str();
    }

  public:
    float v0;
    float v1;
    float v2;
  };

  class Uniform3fvCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform3fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM3FV_REQ>
  {
  public:
    Uniform3fvCommandBufferRequest() = delete;
    Uniform3fvCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<float> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform3fvCommandBufferRequest(const Uniform3fvCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      if (values.size() > 2 && values.size() % 3 == 0) // Check the value size is 3x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << ", ";

      os << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<float> values;
  };

  class Uniform3iCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform3iCommandBufferRequest, COMMAND_BUFFER_UNIFORM3I_REQ>
  {
  public:
    Uniform3iCommandBufferRequest() = delete;
    Uniform3iCommandBufferRequest(uint32_t program, const std::string &location_name, int v0, int v1, int v2)
        : SetUniformCommandBufferRequest(program, location_name)
        , v0(v0)
        , v1(v1)
        , v2(v2)
    {
    }
    Uniform3iCommandBufferRequest(const Uniform3iCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << "), "
         << v0 << ", "
         << v1 << ", "
         << v2 << ")";
      return ss.str();
    }

  public:
    int v0;
    int v1;
    int v2;
  };

  class Uniform3ivCommandBufferRequest final
      : public SetUniformCommandBufferRequest<Uniform3ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM3IV_REQ>
  {
  public:
    Uniform3ivCommandBufferRequest() = delete;
    Uniform3ivCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<int> &values)
        : SetUniformCommandBufferRequest(program, location_name)
        , values(values)
    {
    }
    Uniform3ivCommandBufferRequest(const Uniform3ivCommandBufferRequest &that, bool clone = false)
        : SetUniformCommandBufferRequest(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = SetUniformCommandBufferRequest::serialize();
      if (values.size() > 2 && values.size() % 3 == 0) // Check the value size is 3x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      SetUniformCommandBufferRequest::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << this->locToString() << "), ";

      os << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<int> values;
  };

  template <typename Derived, typename Tv, CommandBufferType Type>
  class Uniform4xCommandBufferRequest : public SetUniformCommandBufferRequest<Derived, Type>
  {
  public:
    Uniform4xCommandBufferRequest(uint32_t program, const std::string &location_name, Tv v0, Tv v1, Tv v2, Tv v3)
        : SetUniformCommandBufferRequest<Derived, Type>(program, location_name)
        , v0(v0)
        , v1(v1)
        , v2(v2)
        , v3(v3)
    {
    }
    Uniform4xCommandBufferRequest(const Derived &that, bool clone = false)
        : SetUniformCommandBufferRequest<Derived, Type>(that, clone)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
        , v3(that.v3)
    {
    }

  public:
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferRequest::toString(line_prefix) << "("
         << this->locToString() << ","
         << v0 << ","
         << v1 << ","
         << v2 << ","
         << v3 << ")";
      return os.str();
    }

  public:
    Tv v0;
    Tv v1;
    Tv v2;
    Tv v3;
  };

  template <typename Derived, typename Tv, CommandBufferType Type>
  class Uniform4xvCommandBufferRequest : public SetUniformCommandBufferRequest<Derived, Type>
  {
  public:
    Uniform4xvCommandBufferRequest(uint32_t program, const std::string &location_name, const std::vector<Tv> &values)
        : SetUniformCommandBufferRequest<Derived, Type>(program, location_name)
        , values(values)
    {
    }
    Uniform4xvCommandBufferRequest(const Derived &that, bool clone = false)
        : SetUniformCommandBufferRequest<Derived, Type>(that, clone)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override final
    {
      auto message = SetUniformCommandBufferRequest<Derived, Type>::serialize();
      if (values.size() > 3 && values.size() % 4 == 0) // Check the value size is 4x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override final
    {
      SetUniformCommandBufferRequest<Derived, Type>::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<Tv>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream os;
      os << TrCommandBufferRequest::toString(line_prefix) << "("
         << this->locToString() << ", ";

      os << "[";
      for (size_t i = 0; i < values.size(); ++i)
      {
        os << values[i];
        if (i < values.size() - 1)
          os << ", ";
      }
      os << "])";
      return os.str();
    }

  public:
    std::vector<Tv> values;
  };

  class Uniform4fCommandBufferRequest final
      : public Uniform4xCommandBufferRequest<Uniform4fCommandBufferRequest, float, COMMAND_BUFFER_UNIFORM4F_REQ>
  {
    using Uniform4xCommandBufferRequest::Uniform4xCommandBufferRequest;
  };

  class Uniform4fvCommandBufferRequest final
      : public Uniform4xvCommandBufferRequest<Uniform4fvCommandBufferRequest, float, COMMAND_BUFFER_UNIFORM4FV_REQ>
  {
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;
  };

  class Uniform4iCommandBufferRequest final
      : public Uniform4xCommandBufferRequest<Uniform4iCommandBufferRequest, int, COMMAND_BUFFER_UNIFORM4I_REQ>
  {
    using Uniform4xCommandBufferRequest::Uniform4xCommandBufferRequest;
  };

  class Uniform4ivCommandBufferRequest final
      : public Uniform4xvCommandBufferRequest<Uniform4ivCommandBufferRequest, int, COMMAND_BUFFER_UNIFORM4IV_REQ>
  {
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;
  };

  template <typename Derived, CommandBufferType Type, size_t matrixSize>
  class UniformMatrixNfvCommandBufferRequest : public SetUniformCommandBufferRequest<Derived, Type>
  {
  public:
    UniformMatrixNfvCommandBufferRequest(uint32_t program,
                                         const std::string &location_name,
                                         bool transpose,
                                         std::vector<float> values = {})
        : SetUniformCommandBufferRequest<Derived, Type>(program, location_name)
        , transpose(transpose)
        , values(values)
    {
    }
    UniformMatrixNfvCommandBufferRequest(const Derived &that, bool clone = false)
        : SetUniformCommandBufferRequest<Derived, Type>(that, clone)
        , transpose(that.transpose)
        , values()
        , computationGraph4values(that.computationGraph4values)
    {
      if (clone)
        values = that.values;
    }

  public:
    size_t count() const
    {
      if (isComputationGraph())
        return 1;
      else
        return values.size() / matrixSize;
    }
    bool isComputationGraph() const
    {
      return computationGraph4values.placeholderId != WebGLMatrixPlaceholderId::NotSet;
    }

  public:
    TrCommandBufferMessage *serialize() override final
    {
      TrCommandBufferMessage *message = SetUniformCommandBufferRequest<Derived, Type>::serialize();
      if (values.size() > matrixSize - 1 && values.size() % matrixSize == 0) [[likely]]
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override final
    {
      SetUniformCommandBufferRequest<Derived, Type>::deserialize(message);

      auto valuesSegment = message.nextSegment();
      if (valuesSegment != nullptr) [[likely]]
        values = valuesSegment->toVec<float>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream values_ss;
      values_ss << "float[" << values.size() << "], "
                << "ComputationGraph=" << (isComputationGraph() ? "Placeholder()" : "None");

      std::stringstream os;
      os << TrCommandBufferRequest::toString(line_prefix) << "("
         << this->locToString() << ", "
         << "values={" << values_ss.str() << ", "
         << "count=" << count() << ", "
         << "transpose=" << (transpose ? "Yes" : "No")
         << ")";
      return os.str();
    }

  public:
    bool transpose;
    std::vector<float> values;
    MatrixComputationGraph computationGraph4values;
  };

  class UniformMatrix2fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix2fvCommandBufferRequest,
                                                    COMMAND_BUFFER_UNIFORM_MATRIX2FV_REQ,
                                                    2 * 2>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;
  };

  class UniformMatrix3fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix3fvCommandBufferRequest,
                                                    COMMAND_BUFFER_UNIFORM_MATRIX3FV_REQ,
                                                    3 * 3>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;
  };

  class UniformMatrix4fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix4fvCommandBufferRequest,
                                                    COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ,
                                                    4 * 4>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;

  public:
    UniformMatrix4fvCommandBufferRequest(uint32_t program,
                                         const std::string &location_name,
                                         bool transpose,
                                         MatrixComputationGraph computationGraph4values)
        : UniformMatrixNfvCommandBufferRequest(program, location_name, transpose)
    {
      this->computationGraph4values = computationGraph4values;
    }
  };
}
