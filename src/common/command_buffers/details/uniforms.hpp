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
        : handedness(MatrixHandedness::MATRIX_LEFT_HANDED),
          placeholderId(WebGLMatrixPlaceholderId::NotSet),
          inverseMatrix(false),
          multiview(false)
    {
    }
    MatrixComputationGraph(WebGLMatrixPlaceholderId placeholder, MatrixHandedness handedness = MatrixHandedness::MATRIX_LEFT_HANDED)
        : placeholderId(placeholder),
          handedness(handedness),
          inverseMatrix(false),
          multiview(false)
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

  class UniformBlockBindingCommandBufferRequest
      : public TrCommandBufferSimpleRequest<UniformBlockBindingCommandBufferRequest>
  {
  public:
    UniformBlockBindingCommandBufferRequest(uint32_t program, uint32_t uniformBlockIndex, uint32_t uniformBlockBinding)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM_BLOCK_BINDING_REQ),
          program(program),
          uniformBlockIndex(uniformBlockIndex),
          uniformBlockBinding(uniformBlockBinding)
    {
    }

  public:
    uint32_t program;
    uint32_t uniformBlockIndex;
    uint32_t uniformBlockBinding;
  };

  class Uniform1fCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform1fCommandBufferRequest>
  {
  public:
    Uniform1fCommandBufferRequest(uint32_t location, float v0)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM1F_REQ),
          location(location),
          v0(v0)
    {
    }

  public:
    uint32_t location;
    float v0;
  };

  class Uniform1fvCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform1fvCommandBufferRequest>
  {
  public:
    Uniform1fvCommandBufferRequest(Uniform1fvCommandBufferRequest &that) : TrCommandBufferSimpleRequest(that), location(that.location)
    {
    }
    Uniform1fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM1FV_REQ),
          location(location),
          values(values)
    {
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

  class Uniform1iCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform1iCommandBufferRequest>
  {
  public:
    Uniform1iCommandBufferRequest(uint32_t location, int v0)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM1I_REQ),
          location(location),
          v0(v0)
    {
    }

  public:
    uint32_t location;
    int v0;
  };

  class Uniform1ivCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform1ivCommandBufferRequest>
  {
  public:
    Uniform1ivCommandBufferRequest(Uniform1ivCommandBufferRequest &that) : TrCommandBufferSimpleRequest(that), location(that.location) {}
    Uniform1ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM1IV_REQ),
          location(location),
          values(values)
    {
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

  public:
    uint32_t location;
    std::vector<int> values;
  };

  class Uniform2fCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform2fCommandBufferRequest>
  {
  public:
    Uniform2fCommandBufferRequest(uint32_t location, float v0, float v1)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM2F_REQ),
          location(location),
          v0(v0),
          v1(v1)
    {
    }

  public:
    uint32_t location;
    float v0;
    float v1;
  };

  class Uniform2fvCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform2fvCommandBufferRequest>
  {
  public:
    Uniform2fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM2FV_REQ),
          location(location),
          values(values)
    {
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

  public:
    uint32_t location;
    std::vector<float> values;
  };

  class Uniform2iCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform2iCommandBufferRequest>
  {
  public:
    Uniform2iCommandBufferRequest(uint32_t location, int v0, int v1)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM2I_REQ),
          location(location),
          v0(v0),
          v1(v1)
    {
    }

  public:
    uint32_t location;
    int v0;
    int v1;
  };

  class Uniform2ivCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform2ivCommandBufferRequest>
  {
  public:
    Uniform2ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM2IV_REQ),
          location(location),
          values(values)
    {
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

  public:
    uint32_t location;
    std::vector<int> values;
  };

  class Uniform3fCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform3fCommandBufferRequest>
  {
  public:
    Uniform3fCommandBufferRequest(uint32_t location, float v0, float v1, float v2)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM3F_REQ),
          location(location),
          v0(v0),
          v1(v1),
          v2(v2)
    {
    }

  public:
    uint32_t location;
    float v0;
    float v1;
    float v2;
  };

  class Uniform3fvCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform3fvCommandBufferRequest>
  {
  public:
    Uniform3fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM3FV_REQ),
          location(location),
          values(values)
    {
    }
    Uniform3fvCommandBufferRequest(Uniform3fvCommandBufferRequest &that) : TrCommandBufferSimpleRequest(that), location(that.location)
    {
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

  public:
    uint32_t location;
    std::vector<float> values;
  };

  class Uniform3iCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform3iCommandBufferRequest>
  {
  public:
    Uniform3iCommandBufferRequest(uint32_t location, int v0, int v1, int v2)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM3I_REQ),
          location(location),
          v0(v0),
          v1(v1),
          v2(v2)
    {
    }

  public:
    uint32_t location;
    int v0;
    int v1;
    int v2;
  };

  class Uniform3ivCommandBufferRequest
      : public TrCommandBufferSimpleRequest<Uniform3ivCommandBufferRequest>
  {
  public:
    Uniform3ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : TrCommandBufferSimpleRequest(COMMAND_BUFFER_UNIFORM3IV_REQ),
          location(location),
          values(values)
    {
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

  public:
    uint32_t location;
    std::vector<int> values;
  };

  template <typename Tb, typename Tv>
  class Uniform4xCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    Uniform4xCommandBufferRequest(CommandBufferType type, uint32_t location, Tv v0, Tv v1, Tv v2, Tv v3)
        : TrCommandBufferRequest(type, sizeof(Tb)),
          location(location),
          v0(v0),
          v1(v1),
          v2(v2),
          v3(v3)
    {
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
    Uniform4xvCommandBufferRequest(Uniform4xvCommandBufferRequest &that)
        : TrCommandBufferRequest(that),
          location(that.location)
    {
    }
    Uniform4xvCommandBufferRequest(CommandBufferType type, uint32_t location, const std::vector<Tv> &values)
        : TrCommandBufferRequest(type, sizeof(Tb)),
          location(location),
          values(values)
    {
    }

  public:
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > 3 && values.size() % 4 == 0) // Check the value size is 4x
        message->addVecSegment(values);
      return message;
    }
    void deserialize(TrCommandBufferMessage &message) override
    {
      auto valuesSegment = message.getSegment(0);
      if (valuesSegment != nullptr)
        values = valuesSegment->toVec<Tv>();
    }

  public:
    uint32_t location;
    std::vector<Tv> values;
  };

  class Uniform4fCommandBufferRequest
      : public Uniform4xCommandBufferRequest<Uniform4fCommandBufferRequest, float>
  {
  public:
    Uniform4fCommandBufferRequest(uint32_t location, float v0, float v1, float v2, float v3)
        : Uniform4xCommandBufferRequest(COMMAND_BUFFER_UNIFORM4F_REQ, location, v0, v1, v2, v3)
    {
    }
  };

  class Uniform4fvCommandBufferRequest
      : public Uniform4xvCommandBufferRequest<Uniform4fvCommandBufferRequest, float>
  {
  public:
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;
    Uniform4fvCommandBufferRequest(uint32_t location, const std::vector<float> &values)
        : Uniform4xvCommandBufferRequest(COMMAND_BUFFER_UNIFORM4FV_REQ, location, values)
    {
    }
  };

  class Uniform4iCommandBufferRequest
      : public Uniform4xCommandBufferRequest<Uniform4iCommandBufferRequest, int>
  {
  public:
    Uniform4iCommandBufferRequest(uint32_t location, int v0, int v1, int v2, int v3)
        : Uniform4xCommandBufferRequest(COMMAND_BUFFER_UNIFORM4I_REQ, location, v0, v1, v2, v3)
    {
    }
  };

  class Uniform4ivCommandBufferRequest : public Uniform4xvCommandBufferRequest<Uniform4ivCommandBufferRequest, int>
  {
  public:
    using Uniform4xvCommandBufferRequest::Uniform4xvCommandBufferRequest;
    Uniform4ivCommandBufferRequest(uint32_t location, const std::vector<int> &values)
        : Uniform4xvCommandBufferRequest(COMMAND_BUFFER_UNIFORM4IV_REQ, location, values)
    {
    }
  };

  template <typename T, size_t matrixSize>
  class UniformMatrixNfvCommandBufferRequest : public TrCommandBufferRequest
  {
  public:
    UniformMatrixNfvCommandBufferRequest(UniformMatrixNfvCommandBufferRequest &that)
        : TrCommandBufferRequest(that),
          location(that.location),
          transpose(that.transpose),
          computationGraph4values(that.computationGraph4values)
    {
    }
    UniformMatrixNfvCommandBufferRequest(CommandBufferType type, uint32_t location, bool transpose)
        : TrCommandBufferRequest(type, sizeof(T)),
          location(location),
          transpose(transpose)
    {
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
    TrCommandBufferMessage *serialize() override
    {
      auto message = new TrCommandBufferMessage(type, size, this);
      if (values.size() > matrixSize - 1 && values.size() % matrixSize == 0) // Check the value size is Nx
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
    bool transpose;
    std::vector<float> values;
    MatrixComputationGraph computationGraph4values;
  };

  class UniformMatrix2fvCommandBufferRequest
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix2fvCommandBufferRequest, 2 * 2>
  {
  public:
    UniformMatrix2fvCommandBufferRequest(UniformMatrix2fvCommandBufferRequest &that)
        : UniformMatrixNfvCommandBufferRequest(that)
    {
    }
    UniformMatrix2fvCommandBufferRequest(uint32_t location, bool transpose, const std::vector<float> &values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX2FV_REQ, location, transpose)
    {
      this->values = values;
    }
  };

  class UniformMatrix3fvCommandBufferRequest
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix3fvCommandBufferRequest, 3 * 3>
  {
  public:
    UniformMatrix3fvCommandBufferRequest(UniformMatrix3fvCommandBufferRequest &that)
        : UniformMatrixNfvCommandBufferRequest(that)
    {
    }
    UniformMatrix3fvCommandBufferRequest(uint32_t location, bool transpose, const std::vector<float> &values)
        : UniformMatrixNfvCommandBufferRequest(COMMAND_BUFFER_UNIFORM_MATRIX3FV_REQ, location, transpose)
    {
      this->values = values;
    }
  };

  class UniformMatrix4fvCommandBufferRequest
      : public UniformMatrixNfvCommandBufferRequest<UniformMatrix4fvCommandBufferRequest, 4 * 4>
  {
  public:
    UniformMatrix4fvCommandBufferRequest(UniformMatrix4fvCommandBufferRequest &that)
        : UniformMatrixNfvCommandBufferRequest(that)
    {
    }
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
