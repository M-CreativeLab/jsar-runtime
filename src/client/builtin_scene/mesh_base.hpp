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
  class Indices : std::vector<T>
  {
  public:
    using std::vector<T>::vector;
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
    using V = void;
  };

  template <>
  class MeshVertexAttributeTraits<float, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x2;
    using V = glm::vec2;
  };

  template <>
  class MeshVertexAttributeTraits<float, 3>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x3;
    using V = glm::vec3;
  };

  template <>
  class MeshVertexAttributeTraits<float, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x4;
    using V = glm::vec4;
  };

  template <>
  class MeshVertexAttributeTraits<uint16_t, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x2;
    using V = glm::u16vec2;
  };

  template <>
  class MeshVertexAttributeTraits<uint16_t, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x4;
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
    virtual VertexFormat format() = 0;
  };

  /**
   * The mesh vertex attribute data, it contains the attribute and values.
   */
  template <typename Format, size_t N>
  class MeshVertexAttributeData : public IMeshVertexAttributeData
  {
    using Traits = MeshVertexAttributeTraits<Format, N>;

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
    void setValues(std::vector<typename Traits::V> values) { this->values = values; }

  public:
    VertexFormat format() override { return Traits::format; }

  public:
    /**
     * The values of the vertex attribute.
     */
    std::vector<typename Traits::V> values{};

  private:
    MeshVertexAttribute<Format, N> attribute_;
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
    Mesh(std::string name) : name(name)
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
     */
    template <typename Format, size_t N>
    inline void insertAttribute(MeshVertexAttributeData<Format, N> &data)
    {
      attributes_[data.attributeId()] = std::make_shared<MeshVertexAttributeData<Format, N>>(data);
    }
    /**
     * Insert a new vertex attribute data.
     *
     * @param attribute The mesh vertex attribute to insert data.
     * @param data The mesh vertex attribute data.
     * @throws std::runtime_error If the vertex attribute data format is invalid.
     */
    template <typename Format, size_t N>
    void insertAttribute(MeshVertexAttribute<Format, N> attribute, MeshVertexAttributeData<Format, N> &data)
    {
      if (attribute.format != data.format())
        throw std::runtime_error("Invalid vertex attribute data format.");
      insertAttribute(data);
    }

  public:
    /**
     * @returns The mesh indices.
     */
    inline Indices<uint32_t> indices() { return indices_; }

  public:
    /**
     * The mesh name.
     */
    std::string name;
    /**
     * The primitive topology of the mesh.
     */
    PrimitiveTopology primitiveTopology = PrimitiveTopology::kTriangleList;

  protected:
    Indices<uint32_t> indices_{};
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
