#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include "./asset.hpp"

namespace builtin_scene
{
  class Primitive2d
  {
  public:
    virtual ~Primitive2d() = default;
  };

  class Primitive3d
  {
  public:
    virtual ~Primitive3d() = default;
  };

  class Measured3d
  {
  public:
    virtual ~Measured3d() = default;

  public:
    virtual float area() = 0;
    virtual float volume() = 0;
  };

  enum class PrimitiveTopology
  {
    kPointList,
    kLineList,
    kLineStrip,
    kTriangleList,
    kTriangleStrip,
    kTriangleFan,
  };

  template <typename T>
  class Indices : public std::vector<T>
  {
  public:
    using std::vector<T>::vector;

  public:
    inline size_t dataSize() { return this->size() * sizeof(T); }
    inline void* dataBuffer() { return this->data(); }
  };

  typedef uint64_t MeshVertexAttributeId;

  /**
   * Supported vertex formats.
   */
  enum class VertexFormat
  {
    kUnknown,
    kFloat32x2,
    kFloat32x3,
    kFloat32x4,
    kUint16x2,
    kUint16x4,
    // More formats?
  };

  /**
   * Traits for vertex attributes, which expose:
   *
   * - `format`: The vertex format.
   * - typename `V`: The type of the vertex attribute value.
   */
  template <typename Format, size_t N>
  class MeshVertexAttributeTraits
  {
  public:
    static const VertexFormat format = VertexFormat::kUnknown;
    static const int formatType = 0;
    static const size_t dataSize = 0;
    using V = void;
  };

  template <>
  class MeshVertexAttributeTraits<float, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x2;
    static const int formatType = WEBGL_FLOAT;
    static const size_t dataSize = sizeof(float) * 2;
    using V = glm::vec2;
  };

  template <>
  class MeshVertexAttributeTraits<float, 3>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x3;
    static const int formatType = WEBGL_FLOAT;
    static const size_t dataSize = sizeof(float) * 3;
    using V = glm::vec3;
  };

  template <>
  class MeshVertexAttributeTraits<float, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x4;
    static const int formatType = WEBGL_FLOAT;
    static const size_t dataSize = sizeof(float) * 4;
    using V = glm::vec4;
  };

  template <>
  class MeshVertexAttributeTraits<uint16_t, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x2;
    static const int formatType = WEBGL_UNSIGNED_INT;
    static const size_t dataSize = sizeof(uint16_t) * 2;
    using V = glm::u16vec2;
  };

  template <>
  class MeshVertexAttributeTraits<uint16_t, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x4;
    static const int formatType = WEBGL_UNSIGNED_INT;
    static const size_t dataSize = sizeof(uint16_t) * 4;
    using V = glm::u16vec4;
  };

  /**
   * The vertex attribute, it contains name, id and format.
   */
  template <typename Format, size_t N>
  class MeshVertexAttribute
  {
    using Traits = MeshVertexAttributeTraits<Format, N>;

  public:
    /**
     * Create a new vertex attribute.
     *
     * @param name The name of the vertex attribute.
     * @param id The id of the vertex attribute.
     * @param format The vertex format.
     */
    MeshVertexAttribute(std::string name, uint64_t id, VertexFormat format)
        : name(name), id(id), format(format)
    {
    }

  public:
    /**
     * The name of the vertex attribute.
     */
    std::string name;
    /**
     * The id of the vertex attribute.
     */
    MeshVertexAttributeId id;
    /**
     * The vertex format.
     */
    VertexFormat format = Traits::format;
  };

  /**
   * The interface of mesh vertex attribute data.
   */
  class IMeshVertexAttributeData
  {
  public:
    virtual ~IMeshVertexAttributeData() = default;

  public:
    /**
     * The vertex format.
     */
    virtual VertexFormat format() = 0;
    /**
     * the number of components per generic vertex attribute. Must be 1, 2, 3, 4. The initial value is 4.
     */
    virtual size_t formatSize() = 0;
    /**
     * the data type of each component in the array, such as `WEBGL_FLOAT`, `WEBGL_UNSIGNED_INT`, etc.
     */
    virtual int formatType() = 0;
    /**
     * Whether fixed-point data values should be normalized (`WEBGL_TRUE`) or converted directly as fixed-point values
     * (`WEBGL_FALSE`) when they are accessed.
     */
    virtual bool normalized() = 0;
    virtual size_t stride() = 0;
    virtual size_t offset() = 0;
    virtual void *dataBuffer() = 0;
    virtual size_t dataSize() = 0;
  };

  /**
   * The mesh vertex attribute data, it contains the attribute and values.
   */
  template <typename Format, size_t N>
  class MeshVertexAttributeData : public IMeshVertexAttributeData
  {
    using Traits = MeshVertexAttributeTraits<Format, N>;
    using ValueType = typename Traits::V;

  public:
    /**
     * Create a new mesh vertex attribute data.
     *
     * @param attribute The mesh vertex attribute.
     * @param values The values of the vertex attribute.
     * @returns The created mesh vertex attribute data.
     */
    static std::shared_ptr<MeshVertexAttributeData<Format, N>> Make(
        MeshVertexAttribute<Format, N> attribute,
        std::vector<ValueType> &values)
    {
      auto instance = std::make_shared<MeshVertexAttributeData<Format, N>>(attribute);
      instance->setValues(values);
      return instance;
    }

  public:
    /**
     * Create a new mesh vertex attribute data.
     *
     * @param attribute The mesh vertex attribute.
     */
    MeshVertexAttributeData(MeshVertexAttribute<Format, N> attribute)
        : attribute_(attribute)
    {
    }
    MeshVertexAttributeData(MeshVertexAttributeData<Format, N> &other)
        : attribute_(other.attribute_), values(other.values)
    {
    }

  public:
    /**
     * @returns The attribute id.
     */
    inline MeshVertexAttributeId attributeId() { return attribute_.id; }
    /**
     * @returns The attribute name.
     */
    inline std::string attributeName() { return attribute_.name; }

  public:
    /**
     * Set the values of the vertex attribute.
     *
     * @param values The values of the vertex attribute.
     */
    void setValues(std::vector<ValueType> values) { this->values = values; }

  public:
    VertexFormat format() override { return Traits::format; }
    size_t formatSize() override { return N; }
    int formatType() override { return Traits::formatType; }
    bool normalized() override { return false; }
    // Stride must be 0, because the data is tightly packed.
    size_t stride() override { return 0; }
    // VBO is used, so the offset is 0.
    size_t offset() override { return 0; }
    void* dataBuffer() override { return values.data(); }
    size_t dataSize() override { return values.size() * Traits::dataSize; }

  public:
    /**
     * The values of the vertex attribute.
     */
    std::vector<ValueType> values{};

  private:
    MeshVertexAttribute<Format, N> attribute_;
  };

  class MeshVertexBuffer
  {
  public:
    MeshVertexBuffer()
    {
    }
    ~MeshVertexBuffer()
    {
      if (cachedData_ != nullptr)
      {
        free(cachedData_);
        cachedData_ = nullptr;
      }
    }

  public:
    template <typename Format, size_t N>
    void insertAttributeData(std::shared_ptr<MeshVertexAttributeData<Format, N>> data)
    {
      assert(data != nullptr);
      attributes_.push_back(data);
    }
    inline size_t dataSize()
    {
      if (cachedSize_ <= 0)
      {
        size_t totalSize = 0;
        for (auto &attribute : attributes_)
          totalSize += attribute->dataSize();
        cachedSize_ = totalSize;
      }
      return cachedSize_;
    }
    inline void *dataBuffer()
    {
      if (cachedData_ == nullptr)
      {
        size_t size = dataSize();
        if (size <= 0)
          return nullptr;

        cachedData_ = malloc(size);
        void *pCurrAttrib = cachedData_;
        for (auto &attribute : attributes_)
        {
          auto data = attribute->dataBuffer();
          auto dataSize = attribute->dataSize();
          memcpy(pCurrAttrib, data, dataSize);
          pCurrAttrib = (void *)((size_t)pCurrAttrib + dataSize);
        }
      }
      return cachedData_;
    }

  private:
    std::vector<std::shared_ptr<IMeshVertexAttributeData>> attributes_{};
    void *cachedData_ = nullptr;
    size_t cachedSize_ = 0;
  };

  class Mesh : public Measured3d
  {
  public:
    /**
     * Where the vertex is located in space.
     */
    static MeshVertexAttribute<float, 3> ATTRIBUTE_POSITION;
    /**
     * The direction the vertex normal is facing in.
     */
    static MeshVertexAttribute<float, 3> ATTRIBUTE_NORMAL;
    /**
     * Texture coordinates for the vertex.
     *
     * Generally `[0.,0.]` is mapped to the top left of the texture, and `[1.,1.]` to the bottom-right.
     */
    static MeshVertexAttribute<float, 2> ATTRIBUTE_UV0;
    /**
     * Alternate texture coordinates for the vertex.
     *
     * Typically, these are used for lightmaps, textures that provide precomputed illumination.
     */
    static MeshVertexAttribute<float, 2> ATTRIBUTE_UV1;
    /**
     * The direction of the vertex tangent. Used for normal mapping.
     */
    static MeshVertexAttribute<float, 4> ATTRIBUTE_TANGENT;
    /**
     * Per vertex coloring.
     */
    static MeshVertexAttribute<float, 4> ATTRIBUTE_COLOR;
    /**
     * Per vertex joint transform matrix weight.
     */
    static MeshVertexAttribute<float, 4> ATTRIBUTE_JOINT_WEIGHTS;
    /**
     * Per vertex joint transform matrix index.
     */
    static MeshVertexAttribute<uint16_t, 4> ATTRIBUTE_JOINT_INDEX;

  public:
    /**
     * Create a new mesh.
     *
     * @param name The name of the mesh.
     */
    Mesh(std::string name, PrimitiveTopology primitiveTopology)
        : name(name),
          primitiveTopology(primitiveTopology)
    {
    }
    virtual ~Mesh() = default;

  public:
    /**
     * Update the indices of the mesh.
     *
     * @param indices The indices of the mesh.
     */
    inline void updateIndices(Indices<uint32_t> indices)
    {
      indices_ = indices;
    }
    /**
     * Insert a new vertex attribute data directly.
     *
     * @param data The mesh vertex attribute data.
     * @throws std::runtime_error If data is `nullptr`.
     */
    template <typename Format, size_t N>
    inline void insertAttribute(std::shared_ptr<MeshVertexAttributeData<Format, N>> data)
    {
      if (data == nullptr)
        throw std::runtime_error("Invalid vertex attribute data.");
      attributes_[data->attributeId()] = data;
      vertexBuffer_.insertAttributeData(data);
    }
    /**
     * Insert a new vertex attribute data.
     *
     * @param attribute The mesh vertex attribute to insert data.
     * @param data The mesh vertex attribute data.
     * @throws std::runtime_error If data is `nullptr`.
     * @throws std::runtime_error If the vertex attribute data format is invalid.
     */
    template <typename Format, size_t N>
    void insertAttribute(MeshVertexAttribute<Format, N> attribute, std::shared_ptr<MeshVertexAttributeData<Format, N>> data)
    {
      if (data == nullptr)
        throw std::runtime_error("Invalid vertex attribute data.");
      if (attribute.format != data->format())
        throw std::runtime_error("Invalid vertex attribute data format.");
      insertAttribute(data);
    }

  public:
    /**
     * @returns The mesh indices.
     */
    inline Indices<uint32_t> &indices() { return indices_; }
    /**
     * @returns The mesh vertex buffer with all attributes.
     */
    inline MeshVertexBuffer &vertexBuffer() { return vertexBuffer_; }
    /**
     * @returns The mesh vertex attributes.
     */
    inline std::unordered_map<MeshVertexAttributeId, std::shared_ptr<IMeshVertexAttributeData>> &attributes() { return attributes_; }

  public:
    /**
     * The mesh name.
     */
    std::string name;
    /**
     * The primitive topology of the mesh.
     */
    PrimitiveTopology primitiveTopology;

  protected:
    Indices<uint32_t> indices_{};
    MeshVertexBuffer vertexBuffer_;
    std::unordered_map<MeshVertexAttributeId, std::shared_ptr<IMeshVertexAttributeData>> attributes_{};
  };

  MeshVertexAttribute<float, 3> Mesh::ATTRIBUTE_POSITION("Vertex_Position", 0, VertexFormat::kFloat32x3);
  MeshVertexAttribute<float, 3> Mesh::ATTRIBUTE_NORMAL("Vertex_Normal", 1, VertexFormat::kFloat32x3);
  MeshVertexAttribute<float, 2> Mesh::ATTRIBUTE_UV0("Vertex_UV0", 2, VertexFormat::kFloat32x2);
  MeshVertexAttribute<float, 2> Mesh::ATTRIBUTE_UV1("Vertex_UV1", 3, VertexFormat::kFloat32x2);
  MeshVertexAttribute<float, 4> Mesh::ATTRIBUTE_TANGENT("Vertex_Tangent", 4, VertexFormat::kFloat32x3);
  MeshVertexAttribute<float, 4> Mesh::ATTRIBUTE_COLOR("Vertex_Color", 5, VertexFormat::kFloat32x3);
  MeshVertexAttribute<float, 4> Mesh::ATTRIBUTE_JOINT_WEIGHTS("Vertex_JointWeights", 6, VertexFormat::kFloat32x4);
  MeshVertexAttribute<uint16_t, 4> Mesh::ATTRIBUTE_JOINT_INDEX("Vertex_JointIndex", 7, VertexFormat::kFloat32x4);
}
