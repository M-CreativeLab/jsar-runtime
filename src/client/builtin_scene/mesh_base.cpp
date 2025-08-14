#include "./mesh_base.hpp"
#include "./meshes.hpp"

namespace builtin_scene
{
  void Vertex::insertAttributeToCompactData(const IVertexAttribute &attribute)
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

  void Vertex::update(const std::vector<MeshVertexAttribute> &enabledAttributes)
  {
    compactData_.clear();
    for (auto &item : enabledAttributes)
    {
      std::visit([this](auto &&attrib)
                 { this->insertAttributeToCompactData(attrib); },
                 item);
    }
  }

  void MeshVertexBuffer::insertVertices(const MeshVertexBuffer &buffer, bool updateAttributes)
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

  void MeshVertexBuffer::enableAttribute(const MeshVertexAttribute &attribute)
  {
    enabledAttributes_.push_back(attribute);
    // Set the stride when enabling the attribute.
    stride_ += std::visit([](auto &&item)
                          { return item.byteLength(); },
                          attribute);
    isDirty_ = true;
  }

  void MeshVertexBuffer::disableAttribute(const MeshVertexAttribute &attribute)
  {
    auto it = std::remove_if(enabledAttributes_.begin(), enabledAttributes_.end(), [&attribute](const MeshVertexAttribute &item)
                             { return item == attribute; });
    enabledAttributes_.erase(it, enabledAttributes_.end());

    // Set the stride when disabling the attribute.
    stride_ -= std::visit([](auto &&item)
                          { return item.byteLength(); },
                          attribute);
    isDirty_ = true;
  }

  void MeshVertexBuffer::clear()
  {
    vertices_.clear();
    enabledAttributes_.clear();
    cachedData_.clear();
    isDirty_ = true;
    stride_ = 0;
  }

  const std::vector<uint8_t> &MeshVertexBuffer::data()
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

  size_t Mesh::iterateEnabledAttributes(std::function<void(const IVertexAttribute &)> callback)
  {
    size_t attribsCount = 0;
    for (auto &item : vertexBuffer_.attributes())
    {
      std::visit([&callback](auto &&attrib)
                 { callback(attrib); },
                 item);
      attribsCount += 1;
    }
    return attribsCount;
  }

  void Mesh::onMesh3dInitialized(const Mesh3d &mesh3d, std::shared_ptr<client_graphics::WebGL2Context>)
  {
    assert(mesh3d.initialized() && "Mesh3d is not initialized.");
  }
}
