#pragma once

#include <memory>
#include <string>
#include <variant>
#include <glm/glm.hpp>
#include <common/command_buffers/webgl_constants.hpp>
#include <client/graphics/webgl_context.hpp>
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

  using PrimitiveTopology = client_graphics::WebGLDrawMode;

  template <typename T>
  class Indices : public std::vector<T>
  {
  public:
    using std::vector<T>::vector;

  public:
    inline void insertIndices(const Indices &indices) { this->insert(this->end(), indices.begin(), indices.end()); }
    inline size_t dataSize() { return this->size() * sizeof(T); }
    inline void *dataBuffer() { return this->data(); }
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
    kUint32,
    // More formats?
  };

  /**
   * The interface for a vertex attribute.
   */
  class IVertexAttribute
  {
  public:
    virtual ~IVertexAttribute() = default;

  public:
    /**
     * Get the id of the vertex attribute.
     */
    virtual MeshVertexAttributeId id() const = 0;
    /**
     * Get the name of the vertex attribute.
     */
    virtual const std::string name() const = 0;
    /**
     * Get the vertex format: float32x2, float32x3, float32x4, uint16x2, uint16x4.
     */
    virtual VertexFormat format() const = 0;
    /**
     * Get the number of components per generic vertex attribute. Must be 1, 2, 3, 4. The initial value is 4.
     */
    virtual size_t size() const = 0;
    /**
     * Get the data type of each component in the array.
     */
    virtual int type() const = 0;
    /**
     * Get whether integer data values should be normalized.
     *
     * For `glVertexAttribPointer`, specifies whether fixed-point data values should be normalized (`GL_TRUE`) or converted directly as
     * fixed-point values (`GL_FALSE`) when they are accessed. This parameter is ignored if type is `GL_FIXED`.
     */
    virtual bool normalized() const = 0;
    /**
     * Get the byte length of this vertex attribute, it's useful to calculate the stride of the vertex buffer.
     */
    virtual size_t byteLength() const = 0;

  public:
    bool is(const IVertexAttribute &attribute) const { return id() == attribute.id(); }
  };

  /**
   * Traits for vertex attributes, which expose:
   *
   * - `format`: The vertex format.
   * - `type`: The WebGL type of the vertex attribute.
   */
  template <typename Format, size_t N>
  class VertexAttributeTraits
  {
  public:
    static const VertexFormat format = VertexFormat::kUnknown;
    static const int type = 0;
  };

  template <>
  class VertexAttributeTraits<float, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x2;
    static const int type = WEBGL_FLOAT;
  };

  template <>
  class VertexAttributeTraits<float, 3>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x3;
    static const int type = WEBGL_FLOAT;
  };

  template <>
  class VertexAttributeTraits<float, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kFloat32x4;
    static const int type = WEBGL_FLOAT;
  };

  template <>
  class VertexAttributeTraits<uint16_t, 2>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x2;
    static const int type = WEBGL_UNSIGNED_INT;
  };

  template <>
  class VertexAttributeTraits<uint16_t, 4>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint16x4;
    static const int type = WEBGL_UNSIGNED_INT;
  };

  template <>
  class VertexAttributeTraits<uint32_t, 1>
  {
  public:
    static const VertexFormat format = VertexFormat::kUint32;
    static const int type = WEBGL_UNSIGNED_INT;
  };

  /**
   * The vertex attribute, it contains name, id and format.
   */
  template <typename Format, size_t N>
  class VertexAttribute : public IVertexAttribute
  {
    using Traits = VertexAttributeTraits<Format, N>;

  public:
    /**
     * Create a new vertex attribute.
     *
     * @param name The name of the vertex attribute.
     * @param id The id of the vertex attribute.
     * @param format The vertex format.
     */
    VertexAttribute(std::string name, uint64_t id, VertexFormat format)
        : name_(name), id_(id), format_(format)
    {
    }

  public:
    MeshVertexAttributeId id() const override { return id_; }
    const std::string name() const override { return name_; }
    VertexFormat format() const override { return format_; }
    size_t size() const override { return N; }
    int type() const override { return Traits::type; }
    bool normalized() const override { return false; }
    size_t byteLength() const override { return N * sizeof(Format); }

  private:
    /**
     * The name of the vertex attribute.
     */
    std::string name_;
    /**
     * The id of the vertex attribute.
     */
    MeshVertexAttributeId id_;
    /**
     * The vertex format.
     */
    VertexFormat format_ = Traits::format;
  };

  using MeshVertexAttribute = std::variant<VertexAttribute<float, 2>,
                                           VertexAttribute<float, 3>,
                                           VertexAttribute<float, 4>,
                                           VertexAttribute<uint16_t, 4>>;

  // Custom comparison operator for `MeshVertexAttribute`.
  inline bool operator==(const MeshVertexAttribute &lhs, const MeshVertexAttribute &rhs)
  {
    return std::visit([&](auto &&attrib)
                      { return std::visit([&](auto &&attrib2)
                                          { return attrib.id() == attrib2.id(); }, rhs); }, lhs);
  }

  class Vertex
  {
    friend class MeshVertexBuffer;

  public:
    inline static auto ATTRIBUTE_POSITION = VertexAttribute<float, 3>("position", 0, VertexFormat::kFloat32x3);
    inline static auto ATTRIBUTE_NORMAL = VertexAttribute<float, 3>("normal", 1, VertexFormat::kFloat32x3);
    inline static auto ATTRIBUTE_UV0 = VertexAttribute<float, 2>("texCoord", 2, VertexFormat::kFloat32x2);
    inline static auto ATTRIBUTE_UV1 = VertexAttribute<float, 2>("texCoord1", 3, VertexFormat::kFloat32x2);
    inline static auto ATTRIBUTE_TANGENT = VertexAttribute<float, 4>("tangent", 4, VertexFormat::kFloat32x4);
    inline static auto ATTRIBUTE_COLOR = VertexAttribute<float, 4>("color", 5, VertexFormat::kFloat32x4);
    inline static auto ATTRIBUTE_JOINT_WEIGHTS = VertexAttribute<float, 4>("jointWeights", 6, VertexFormat::kFloat32x4);
    inline static auto ATTRIBUTE_JOINT_INDEX = VertexAttribute<uint16_t, 4>("jointIndex", 7, VertexFormat::kUint16x4);

  public:
    Vertex() = default;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
        : position(position), normal(normal), uv0(uv)
    {
    }

  public:
    size_t size() const { return compactData_.size(); }
    const void *data() const { return compactData_.data(); }

  private:
    void insertAttributeToCompactData(const IVertexAttribute &attribute)
    {
      if (attribute.is(ATTRIBUTE_POSITION))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&position),
                            reinterpret_cast<const uint8_t *>(&position) + sizeof(position));
      else if (attribute.is(ATTRIBUTE_NORMAL))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&normal),
                            reinterpret_cast<const uint8_t *>(&normal) + sizeof(normal));
      else if (attribute.is(ATTRIBUTE_UV0))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&uv0),
                            reinterpret_cast<const uint8_t *>(&uv0) + sizeof(uv0));
      else if (attribute.is(ATTRIBUTE_UV1))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&uv1),
                            reinterpret_cast<const uint8_t *>(&uv1) + sizeof(uv1));
      else if (attribute.is(ATTRIBUTE_TANGENT))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&tangent),
                            reinterpret_cast<const uint8_t *>(&tangent) + sizeof(tangent));
      else if (attribute.is(ATTRIBUTE_COLOR))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&color),
                            reinterpret_cast<const uint8_t *>(&color) + sizeof(color));
      else if (attribute.is(ATTRIBUTE_JOINT_WEIGHTS))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&jointWeights),
                            reinterpret_cast<const uint8_t *>(&jointWeights) + sizeof(jointWeights));
      else if (attribute.is(ATTRIBUTE_JOINT_INDEX))
        compactData_.insert(compactData_.end(),
                            reinterpret_cast<const uint8_t *>(&jointIndex),
                            reinterpret_cast<const uint8_t *>(&jointIndex) + sizeof(jointIndex));
    }
    void update(const std::vector<MeshVertexAttribute> &enabledAttributes)
    {
      compactData_.clear();
      for (auto &item : enabledAttributes)
      {
        std::visit([this](auto &&attrib)
                   { this->insertAttributeToCompactData(attrib); }, item);
      }
    }

  public:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv0;
    glm::vec2 uv1;
    glm::vec4 tangent;
    glm::vec4 color;
    glm::vec4 jointWeights;
    glm::u16vec4 jointIndex;
    // TODO: support custom attributes?

  private:
    std::vector<uint8_t> compactData_;
  };

  class MeshVertexBuffer
  {
  public:
    MeshVertexBuffer() = default;
    MeshVertexBuffer(const MeshVertexBuffer &) = delete;

  public:
    /**
     * Inserts a vertex to the buffer.
     *
     * @param vertex The vertex data to add.
     */
    void insertVertex(Vertex &vertex)
    {
      vertices_.push_back(vertex);
      isDirty_ = true;
    }

    /**
     * Inserts vertices from another buffer.
     *
     * @param buffer The buffer to insert vertices from.
     * @param updateAttributes Whether to update the attributes configuration.
     */
    void insertVertices(const MeshVertexBuffer &buffer, bool updateAttributes)
    {
      for (auto &vertex : buffer.vertices_)
        vertices_.push_back(vertex);

      if (updateAttributes)
      {
        enabledAttributes_ = buffer.enabledAttributes_;
        stride_ = buffer.stride_;
        isDirty_ = true;
      }
    }

    /**
     * Enable a vertex attribute.
     */
    void enableAttribute(const MeshVertexAttribute &attribute)
    {
      enabledAttributes_.push_back(attribute);
      // Set the stride when enabling the attribute.
      stride_ += std::visit([](auto &&item)
                            { return item.byteLength(); }, attribute);
      isDirty_ = true;
    }

    /**
     * Disable a vertex attribute.
     */
    void disableAttribute(const MeshVertexAttribute &attribute)
    {
      auto it = std::remove_if(enabledAttributes_.begin(), enabledAttributes_.end(),
                               [&attribute](const MeshVertexAttribute &item)
                               { return item == attribute; });
      enabledAttributes_.erase(it, enabledAttributes_.end());

      // Set the stride when disabling the attribute.
      stride_ -= std::visit([](auto &&item)
                            { return item.byteLength(); }, attribute);
      isDirty_ = true;
    }

    /**
     * Get the enabled vertex attributes.
     */
    inline const std::vector<MeshVertexAttribute> &attributes() const { return enabledAttributes_; }

    /**
     * Get the number of vertices in the buffer.
     */
    inline size_t vertexCount() const { return vertices_.size(); }

    /**
     * Get the stride of the vertex buffer.
     */
    inline size_t stride() const { return stride_; }

    /**
     * Clear the vertex buffer.
     */
    inline void clear()
    {
      vertices_.clear();
      enabledAttributes_.clear();
      cachedData_.clear();
      isDirty_ = true;
      stride_ = 0;
    }

    /**
     * Get a pointer to the interleaved vertex data.
     *
     * @return A pointer to the vertex data.
     */
    const std::vector<uint8_t> &data()
    {
      if (!isDirty_ && cachedData_.size() > 0)
        return cachedData_;

      auto &enabledAttribs = attributes();
      std::vector<uint8_t> data;
      for (auto &vertex : vertices_)
      {
        vertex.update(enabledAttribs); // Update the vertex based on the enabled attributes.

        const uint8_t *vertexData = reinterpret_cast<const uint8_t *>(vertex.data());
        data.insert(data.end(), vertexData, vertexData + vertex.size());
      }
      cachedData_ = data;
      isDirty_ = false;
      return cachedData_;
    }

  private:
    std::vector<Vertex> vertices_;                       // Container to store vertices
    std::vector<MeshVertexAttribute> enabledAttributes_; // Enabled vertex attributes
    std::vector<uint8_t> cachedData_;                    // Cached interleaved vertex data
    bool isDirty_ = true;                                // Whether the buffer is dirty
    size_t stride_ = 0;                                  // The stride of the vertex buffer
  };

  class Mesh : public Measured3d
  {
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
     * Insert the vertex to the mesh.
     *
     * @param vertex The vertex to insert.
     */
    inline void insertVertex(Vertex &vertex)
    {
      vertexBuffer_.insertVertex(vertex);
    }
    /**
     * Insert a new vertex from the given position, normal, and uv.
     *
     * @param position The position of the vertex.
     * @param normal The normal of the vertex.
     * @param uv The uv of the vertex.
     */
    inline void insertVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv)
    {
      Vertex vertex(position, normal, uv);
      insertVertex(vertex);
    }
    /**
     * Enable a vertex attribute.
     *
     * @param attribute The vertex attribute to enable.
     */
    inline void enableAttribute(const MeshVertexAttribute &attribute)
    {
      vertexBuffer_.enableAttribute(attribute);
    }
    /**
     * Disable a vertex attribute.
     *
     * @param attribute The vertex attribute to disable.
     */
    inline void disableAttribute(const MeshVertexAttribute &attribute)
    {
      vertexBuffer_.disableAttribute(attribute);
    }
    /**
     * The attributes stride is the sum of all enabled attributes byte length, that's used by the
     * graphics API to get the correct attribute data.
     */
    inline size_t attributesStride() const { return vertexBuffer_.stride(); }

  public:
    /**
     * @returns The mesh indices.
     */
    inline const Indices<uint32_t> &indices() const { return indices_; }
    /**
     * @returns The mesh vertex buffer with all attributes.
     */
    inline MeshVertexBuffer &vertexBuffer() { return vertexBuffer_; }
    /**
     * Iterate the enabled attributes of the mesh.
     *
     * @param callback The callback to call for each attribute.
     * @returns The number of enabled attributes.
     */
    inline size_t iterateEnabledAttributes(std::function<void(const IVertexAttribute &)> callback)
    {
      size_t attribsCount = 0;
      for (auto &item : vertexBuffer_.attributes())
      {
        std::visit([&callback](auto &&attrib)
                   { callback(attrib); }, item);
        attribsCount += 1;
      }
      return attribsCount;
    }
    /**
     * @returns Whether the mesh is dirty to update the vertex buffer data.
     */
    inline bool isDirty() const { return isDirty_; }
    /**
     * Set the mesh dirty flag: `true` to update the vertex buffer data, `false` otherwise.
     */
    inline void setDirty(bool dirty) { isDirty_ = dirty; }

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
    bool isDirty_ = true;
  };
}
