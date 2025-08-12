#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./mesh_base.hpp"
#include "./meshes.hpp"
#include "./gaussian_splats_mesh.hpp"

namespace builtin_scene
{
  // Static member for empty indices
  const Indices<uint32_t> GaussianSplatsMesh::emptyIndices_;

  GaussianSplatsMesh::GaussianSplatsMesh()
      : meshes::Splat()
      , needsRebuild_(false)
      , needsSorting_(false)
      , bufferInitialized_(false)
      , textureInitialized_(false)
  {
  }

  void GaussianSplatsMesh::addSplatsEntity(ecs::EntityId entityId)
  {
    splatEntities_.insert(entityId);
    needsRebuild_ = true;
    needsSorting_ = true;
  }

  void GaussianSplatsMesh::removeSplatsEntity(ecs::EntityId entityId)
  {
    auto it = splatEntities_.find(entityId);
    if (it != splatEntities_.end())
    {
      splatEntities_.erase(it);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::updateSplatsEntity(ecs::EntityId entityId)
  {
    if (splatEntities_.count(entityId) > 0)
    {
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::setupSplatBuffer(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                            std::shared_ptr<client_graphics::WebGLVertexArray> vao)
  {
    if (!glContext || bufferInitialized_)
      return;

    // Create the instance buffer for splat data
    splatInstanceBuffer_ = glContext->createBuffer();

    if (splatInstanceBuffer_)
    {
      bufferInitialized_ = true;
    }
  }

  void GaussianSplatsMesh::updateSplatBuffer(std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    if (!glContext ||
        !bufferInitialized_ ||
        sortedSplats_.empty() ||
        !isDirty())
      return;

    // Prepare a contiguous array of sorted indices (only uint32_t values now)
    std::vector<uint32_t> indexData;
    indexData.reserve(sortedSplats_.size());

    for (const auto &splat : sortedSplats_)
    {
      indexData.push_back(splat.sortedIndex);
    }

    // Upload to GPU
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatInstanceBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          indexData.size() * sizeof(uint32_t),
                          indexData.data(),
                          client_graphics::WebGLBufferUsage::kDynamicDraw);
    setDirty(false);

    DEBUG("GaussianSplatsMesh", "Updated GPU buffer with %zu sorted indices", sortedSplats_.size());
  }

  void GaussianSplatsMesh::updateSplatTexture(std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    if (!glContext || splatTextureData_.empty())
      return;

    // Create texture if not initialized
    if (!textureInitialized_)
    {
      splatDataTexture_ = glContext->createTexture();
      if (!splatDataTexture_)
        return;
      textureInitialized_ = true;
    }

    // Calculate texture dimensions
    // Each splat needs 56 bytes (14 floats): position(3) + color(3) + opacity(1) + scale(3) + rotation(4)
    // We'll pack this into RGBA32F format, so we need ceil(14/4) = 4 texels per splat
    size_t texelsPerSplat = 4; // 4 RGBA32F texels = 16 floats, enough for 14 floats + 2 padding
    size_t totalTexels = splatTextureData_.size() * texelsPerSplat;

    // Choose texture dimensions (prefer square-ish textures)
    size_t textureWidth = static_cast<size_t>(std::ceil(std::sqrt(static_cast<float>(totalTexels))));
    size_t textureHeight = (totalTexels + textureWidth - 1) / textureWidth; // Ceiling division

    // Ensure minimum size
    textureWidth = std::max(textureWidth, size_t(1));
    textureHeight = std::max(textureHeight, size_t(1));

    // Prepare texture data
    std::vector<float> textureData(textureWidth * textureHeight * 4, 0.0f); // RGBA format

    for (size_t i = 0; i < splatTextureData_.size(); ++i)
    {
      const auto &splat = splatTextureData_[i];
      size_t baseIndex = i * texelsPerSplat * 4; // 4 components per texel

      // Texel 0: position + opacity
      textureData[baseIndex + 0] = splat.position.x;
      textureData[baseIndex + 1] = splat.position.y;
      textureData[baseIndex + 2] = splat.position.z;
      textureData[baseIndex + 3] = splat.opacity;

      // Texel 1: color + scale.x
      textureData[baseIndex + 4] = splat.color.r;
      textureData[baseIndex + 5] = splat.color.g;
      textureData[baseIndex + 6] = splat.color.b;
      textureData[baseIndex + 7] = splat.scale.x;

      // Texel 2: scale.yz + rotation.xy
      textureData[baseIndex + 8] = splat.scale.y;
      textureData[baseIndex + 9] = splat.scale.z;
      textureData[baseIndex + 10] = splat.rotation.x;
      textureData[baseIndex + 11] = splat.rotation.y;

      // Texel 3: rotation.zw + padding
      textureData[baseIndex + 12] = splat.rotation.z;
      textureData[baseIndex + 13] = splat.rotation.w;
      textureData[baseIndex + 14] = 0.0f; // padding
      textureData[baseIndex + 15] = 0.0f; // padding
    }

    // Upload texture data
    glContext->bindTexture(client_graphics::WebGLTextureTarget::kTexture2D, splatDataTexture_);
    glContext->texImage2D(client_graphics::WebGLTexture2DTarget::kTexture2D,
                          0,              // level
                          WEBGL2_RGBA32F, // internal format
                          textureWidth,
                          textureHeight,
                          0,           // border
                          WEBGL_RGBA,  // format
                          WEBGL_FLOAT, // type
                          textureData.data());

    // Set texture parameters for point sampling (no filtering)
    glContext->texParameteri(client_graphics::WebGLTextureTarget::kTexture2D,
                             WEBGL_TEXTURE_MIN_FILTER,
                             WEBGL_NEAREST);
    glContext->texParameteri(client_graphics::WebGLTextureTarget::kTexture2D,
                             WEBGL_TEXTURE_MAG_FILTER,
                             WEBGL_NEAREST);
    glContext->texParameteri(client_graphics::WebGLTextureTarget::kTexture2D,
                             WEBGL_TEXTURE_WRAP_S,
                             WEBGL_CLAMP_TO_EDGE);
    glContext->texParameteri(client_graphics::WebGLTextureTarget::kTexture2D,
                             WEBGL_TEXTURE_WRAP_T,
                             WEBGL_CLAMP_TO_EDGE);

    DEBUG("GaussianSplatsMesh", "Updated splat data texture: %zu splats, %zux%zu texture", splatTextureData_.size(), textureWidth, textureHeight);
  }

  void GaussianSplatsMesh::onMesh3dInitialized(const Mesh3d &mesh3d,
                                               std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    // Call parent implementation first
    Mesh::onMesh3dInitialized(mesh3d, glContext);

    // Store the glContext for iterateInstanceAttributes
    glContext_ = glContext;

    // Initialize the splat buffer
    setupSplatBuffer(glContext, mesh3d.vertexArrayObject());
  }

  size_t GaussianSplatsMesh::iterateInstanceAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                       std::function<void(const IVertexAttribute &,
                                                                          int,
                                                                          size_t,
                                                                          size_t)> callback) const
  {
    // We need a weak reference to glContext for attribute location queries
    auto glContext = glContext_.lock();
    if (glContext == nullptr)
      return 0;

    size_t attribsCount = 0;
    size_t offset = 0;

    for (size_t i = 0; i < INSTANCE_ATTRIBUTES.size(); i++)
    {
      auto &name = INSTANCE_ATTRIBUTES[i];
      auto attribLocation = glContext->getAttribLocation(program, name);
      if (attribLocation.has_value())
      {
        auto instanceIndex = attribLocation.value().index.value_or(-1);
        std::unique_ptr<IVertexAttribute> attrib = nullptr;

        // Configure based on attribute name and type
        if (name == "splatSortedIndex")
        {
          // uint32 attribute for texture index
          attrib = make_unique<VertexAttribute<uint32_t, 1>>(name, instanceIndex, VertexFormat::kUint32);
        }
        else
        {
          assert(false && "Unknown splat instance attribute name.");
        }

        assert(attrib != nullptr);
        callback(*attrib, instanceIndex, STRIDE, offset);
        offset += attrib->byteLength();
        attribsCount += 1;
      }
      else
      {
        cerr << "The splat instance attribute " << name << " is not found." << endl;
      }
    }

    return attribsCount;
  }
}
