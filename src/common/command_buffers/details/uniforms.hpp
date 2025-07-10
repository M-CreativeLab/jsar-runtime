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

  class Uniform1fCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform1fCommandBufferRequest, COMMAND_BUFFER_UNIFORM1F_REQ>
  {
  public:
    Uniform1fCommandBufferRequest(uint32_t location, float v0)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
    {
    }
    Uniform1fCommandBufferRequest(const Uniform1fCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , v0(that.v0)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << location << ","
         << v0 << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    float v0;
  };

  class Uniform1fvCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform1fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM1FV_REQ>
  {
  public:
    Uniform1fvCommandBufferRequest() = delete;
    Uniform1fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform1fvCommandBufferRequest(const Uniform1fvCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 0)
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }

  public:
    uint32_t location;
    std::vector<float> values;
  };

  class Uniform1iCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform1iCommandBufferRequest, COMMAND_BUFFER_UNIFORM1I_REQ>
  {
  public:
    Uniform1iCommandBufferRequest() = delete;
    Uniform1iCommandBufferRequest(uint32_t location, int v0)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
    {
    }
    Uniform1iCommandBufferRequest(const Uniform1iCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , v0(that.v0)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),"
         << v0 << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    int v0;
  };

  class Uniform1ivCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform1ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM1IV_REQ>
  {
  public:
    Uniform1ivCommandBufferRequest() = delete;
    Uniform1ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform1ivCommandBufferRequest(const Uniform1ivCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 0)
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<int> values;
  };

  class Uniform2fCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform2fCommandBufferRequest, COMMAND_BUFFER_UNIFORM2F_REQ>
  {
  public:
    Uniform2fCommandBufferRequest() = delete;
    Uniform2fCommandBufferRequest(uint32_t location, float v0, float v1)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
        , v1(v1)
    {
    }
    Uniform2fCommandBufferRequest(const Uniform2fCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , v0(that.v0)
        , v1(that.v1)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),"
         << v0 << ","
         << v1 << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    float v0;
    float v1;
  };

  class Uniform2fvCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform2fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM2FV_REQ>
  {
  public:
    Uniform2fvCommandBufferRequest() = delete;
    Uniform2fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform2fvCommandBufferRequest(const Uniform2fvCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 1 && values.size() % 2 == 0) // Check the value size is 2x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<float> values;
  };

  class Uniform2iCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform2iCommandBufferRequest, COMMAND_BUFFER_UNIFORM2I_REQ>
  {
  public:
    Uniform2iCommandBufferRequest() = delete;
    Uniform2iCommandBufferRequest(uint32_t location, int v0, int v1)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
        , v1(v1)
    {
    }
    Uniform2iCommandBufferRequest(const Uniform2iCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , v0(that.v0)
        , v1(that.v1)
    {
    }

    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),"
         << v0 << ","
         << v1 << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    int v0;
    int v1;
  };

  class Uniform2ivCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform2ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM2IV_REQ>
  {
  public:
    Uniform2ivCommandBufferRequest() = delete;
    Uniform2ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform2ivCommandBufferRequest(const Uniform2ivCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 1 && values.size() % 2 == 0) // Check the value size is 2x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<int> values;
  };

  class Uniform3fCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform3fCommandBufferRequest, COMMAND_BUFFER_UNIFORM3F_REQ>
  {
  public:
    Uniform3fCommandBufferRequest() = delete;
    Uniform3fCommandBufferRequest(uint32_t location, float v0, float v1, float v2)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
        , v1(v1)
        , v2(v2)
    {
    }
    Uniform3fCommandBufferRequest(const Uniform3fCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
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
    uint32_t location;
    float v0;
    float v1;
    float v2;
  };

  class Uniform3fvCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform3fvCommandBufferRequest, COMMAND_BUFFER_UNIFORM3FV_REQ>
  {
  public:
    Uniform3fvCommandBufferRequest() = delete;
    Uniform3fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform3fvCommandBufferRequest(const Uniform3fvCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 2 && values.size() % 3 == 0) // Check the value size is 3x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<float> values;
  };

  class Uniform3iCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform3iCommandBufferRequest, COMMAND_BUFFER_UNIFORM3I_REQ>
  {
  public:
    Uniform3iCommandBufferRequest() = delete;
    Uniform3iCommandBufferRequest(uint32_t location, int v0, int v1, int v2)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , v0(v0)
        , v1(v1)
        , v2(v2)
    {
    }
    Uniform3iCommandBufferRequest(const Uniform3iCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that, clone)
        , location(that.location)
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
    uint32_t location;
    int v0;
    int v1;
    int v2;
  };

  class Uniform3ivCommandBufferRequest final
      : public TrCommandBufferSimpleRequest<Uniform3ivCommandBufferRequest, COMMAND_BUFFER_UNIFORM3IV_REQ>
  {
  public:
    Uniform3ivCommandBufferRequest() = delete;
    Uniform3ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest()
        , location(location)
        , values(values)
    {
    }
    Uniform3ivCommandBufferRequest(const Uniform3ivCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferSimpleRequest(that)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 2 && values.size() % 3 == 0) // Check the value size is 3x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<int>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferSimpleRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<int> values;
  };

  template <typename Tb, typename Tv>
  class Uniform4xCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    Uniform4xCommandBufferRequest(CommandBufferType type, uint32_t location, Tv v0, Tv v1, Tv v2, Tv v3)
        : TrCommandBufferRequest(type, sizeof(Tb))
        , location(location)
        , v0(v0)
        , v1(v1)
        , v2(v2)
        , v3(v3)
    {
    }
    Uniform4xCommandBufferRequest(const Uniform4xCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferRequest(that, clone)
        , location(that.location)
        , v0(that.v0)
        , v1(that.v1)
        , v2(that.v2)
        , v3(that.v3)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override final
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override final
    {
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),"
         << v0 << ","
         << v1 << ","
         << v2 << ","
         << v3 << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    Tv v0;
    Tv v1;
    Tv v2;
    Tv v3;
  };

  template <typename Tb, typename Tv>
  class Uniform4xvCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    Uniform4xvCommandBufferRequest(CommandBufferType type, uint32_t location, const std::vector<Tv> &values)
        : TrCommandBufferRequest(type, sizeof(Tb))
        , location(location)
        , values(values)
    {
    }
    Uniform4xvCommandBufferRequest(const Uniform4xvCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferRequest(that, clone)
        , location(that.location)
        , values()
    {
      if (clone)
        values = that.values;
    }

  public:
    TrCommandBufferMessage *serialize() override final
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 3 && values.size() % 4 == 0) // Check the value size is 4x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override final
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<Tv>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream ss;
      ss << TrCommandBufferRequest::toString(line_prefix) << "("
         << "Loc(" << location << "),";

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
    uint32_t location;
    std::vector<Tv> values;
  };

  class Uniform4fCommandBufferRequest final
      : public Uniform4xCommandBufferRequest<Uniform4fCommandBufferRequest, float>
  {
    using Uniform4xCommandBufferRequest::Uniform4xCommandBufferRequest;

  public:
    Uniform4fCommandBufferRequest(uint32_t location, float v0, float v1, float v2, float v3)
        : Uniform4xCommandBufferRequest(COMMAND_BUFFER_UNIFORM4F_REQ, location, v0, v1, v2, v3)
    {
    }
  };

  class Uniform4fvCommandBufferRequest final
      : public Uniform4xvCommandBufferRequest<Uniform4fvCommandBufferRequest, float>
  {
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;

  public:
    Uniform4fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : Uniform4xvCommandBufferRequest(COMMAND_BUFFER_UNIFORM4FV_REQ, location, values)
    {
    }
  };

  class Uniform4iCommandBufferRequest final
      : public Uniform4xCommandBufferRequest<Uniform4iCommandBufferRequest, int>
  {
    using Uniform4xCommandBufferRequest::Uniform4xCommandBufferRequest;

  public:
    Uniform4iCommandBufferRequest(uint32_t location, int v0, int v1, int v2, int v3)
        : Uniform4xCommandBufferRequest(COMMAND_BUFFER_UNIFORM4I_REQ, location, v0, v1, v2, v3)
    {
    }
  };

  class Uniform4ivCommandBufferRequest final
      : public Uniform4xvCommandBufferRequest<Uniform4ivCommandBufferRequest, int>
  {
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;

  public:
    Uniform4ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : Uniform4xvCommandBufferRequest(COMMAND_BUFFER_UNIFORM4IV_REQ, location, values)
    {
    }
  };

  template <typename T, size_t matrixSize>
  class UniformMatrixNfvCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    UniformMatrixNfvCommandBufferRequest(CommandBufferType type, uint32_t location, bool transpose)
        : TrCommandBufferRequest(type, sizeof(T))
        , location(location)
        , transpose(transpose)
    {
    }
    UniformMatrixNfvCommandBufferRequest(const UniformMatrixNfvCommandBufferRequest &that, bool clone = false)
        : TrCommandBufferRequest(that, clone)
        , location(that.location)
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
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > matrixSize - 1 && values.size() % matrixSize == 0) // Check the value size is Nx
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override final
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<float>();
    }
    std::string toString(const char *line_prefix) const override
    {
      std::stringstream values_ss;
      values_ss << "float[" << values.size() << "], "
                << "ComputationGraph=" << (isComputationGraph() ? "Placeholder()" : "None");

      std::stringstream ss;
      ss << TrCommandBufferRequest::toString(line_prefix) << "("
         << location << ", "
         << "values={" << values_ss.str() << ", "
         << "count=" << count() << ", "
         << "transpose=" << (transpose ? "Yes" : "No")
         << ")";
      return ss.str();
    }

  public:
    uint32_t location;
    bool transpose;
    std::vector<float> values;
    MatrixComputationGraph computationGraph4values;
  };

  class UniformMatrix2fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix2fvCommandBufferRequest, 2 * 2>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;

  public:
    UniformMatrix2fvCommandBufferRequest(uint32_t location, bool transpose, const std::vector<float> &values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX2FV_REQ, location, transpose)
    {
      this->values = values;
    }
  };

  class UniformMatrix3fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix3fvCommandBufferRequest, 3 * 3>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;

  public:
    UniformMatrix3fvCommandBufferRequest(uint32_t location, bool transpose, const std::vector<float> &values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX3FV_REQ, location, transpose)
    {
      this->values = values;
    }
  };

  class UniformMatrix4fvCommandBufferRequest final
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix4fvCommandBufferRequest, 4 * 4>
  {
    using UniformMatrixNfvCommandBufferRequest::UniformMatrixNfvCommandBufferRequest;

  public:
    UniformMatrix4fvCommandBufferRequest(uint32_t location, bool transpose)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ, location, transpose)
    {
    }
    UniformMatrix4fvCommandBufferRequest(uint32_t location, bool transpose, const std::vector<float> &values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ, location, transpose)
    {
      this->values = values;
    }
    UniformMatrix4fvCommandBufferRequest(uint32_t location, bool transpose, MatrixComputationGraph computationGraph4values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX4FV_REQ, location, transpose)
    {
      this->computationGraph4values = computationGraph4values;
    }
  };
}
