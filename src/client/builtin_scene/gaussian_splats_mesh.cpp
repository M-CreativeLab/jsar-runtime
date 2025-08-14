#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./mesh_base.hpp"
#include "./meshes.hpp"
#include "./gaussian_splats_mesh.hpp"
#include "./compressed_splats.hpp"

namespace builtin_scene
{
  using namespace std;
  using namespace client_graphics;

  // Static member for empty indices
  const Indices<uint32_t> GaussianSplatsMesh::emptyIndices_;

  GaussianSplatsMesh::GaussianSplatsMesh()
      : meshes::Splat()
      , needsRebuild_(false)
      , needsSorting_(false)
      , bufferInitialized_(false)
      , textureInitialized_(false)
      , needsTextureUpdate_(false)
  {
  }

  void GaussianSplatsMesh::addSplatsEntity(ecs::EntityId entityId)
  {
    // Check if entity already exists to avoid duplicates
    auto it = find(splatEntities_.begin(), splatEntities_.end(), entityId);
    if (it == splatEntities_.end())
    {
      splatEntities_.push_back(entityId);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::removeSplatsEntity(ecs::EntityId entityId)
  {
    auto it = find(splatEntities_.begin(), splatEntities_.end(), entityId);
    if (it != splatEntities_.end())
    {
      splatEntities_.erase(it);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::updateSplatsEntity(ecs::EntityId entityId)
  {
    auto it = find(splatEntities_.begin(), splatEntities_.end(), entityId);
    if (it != splatEntities_.end())
    {
      // Entity exists, mark for rebuild
      needsRebuild_ = true;
      needsSorting_ = true;
    }
    else
    {
      // Entity doesn't exist, add it
      splatEntities_.push_back(entityId);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::setupSplatBuffer(shared_ptr<WebGL2Context> glContext, shared_ptr<WebGLVertexArray> vao)
  {
    if (!glContext || bufferInitialized_)
      return;

    // Create the instance buffer for splat data
    splatInstanceBuffer_ = glContext->createBuffer();
    if (splatInstanceBuffer_) [[likely]]
      bufferInitialized_ = true;
  }

  void GaussianSplatsMesh::updateSplatBuffer(shared_ptr<WebGL2Context> glContext)
  {
    if (!glContext ||
        !bufferInitialized_ ||
        sortedSplats_.empty() ||
        !isDirty())
      return;

    // Prepare a contiguous array of sorted indices (only uint32_t values now)
    vector<uint32_t> indexData;
    indexData.reserve(sortedSplats_.size());

    for (const auto &splat : sortedSplats_)
      indexData.push_back(splat.index);

    // Upload to GPU
    glContext->bindBuffer(WebGLBufferBindingTarget::kArrayBuffer, splatInstanceBuffer_);
    glContext->bufferData(WebGLBufferBindingTarget::kArrayBuffer,
                          indexData.size() * sizeof(uint32_t),
                          indexData.data(),
                          WebGLBufferUsage::kDynamicDraw);
    setDirty(false);

    DEBUG("GaussianSplatsMesh", "Updated GPU buffer with %zu sorted indices", sortedSplats_.size());
  }

  void GaussianSplatsMesh::updateSplatTextures(shared_ptr<WebGL2Context> glContext)
  {
    if (!glContext || compressedSplatData_.empty())
      return;

    // Calculate texture dimensions using compressed method
    auto textureSize = compressed_splat_utils::getTextureSize(static_cast<uint32_t>(compressedSplatData_.size()));
    uint32_t width = textureSize[0];  // 1024
    uint32_t height = textureSize[1]; // power of 2 height
    uint32_t maxSplats = textureSize[2];

    // Create texture2DArray if not initialized
    if (!textureInitialized_)
    {
      compressedSplatsTexture_ = glContext->createTexture();

      if (!compressedSplatsTexture_)
        return;
      textureInitialized_ = true;
    }

    // Prepare texture data array for single layer
    vector<float> splatData(maxSplats * 4, 0.0f); // Single texel: compressed_pos, compressed_scale, compressed_quat, compressed_color

    // Fill texture data from compressed splats
    for (size_t i = 0; i < compressedSplatData_.size(); ++i)
    {
      const auto &compressed = compressedSplatData_[i];

      // Single texel data
      splatData[i * 4 + 0] = compressed.word[0]; // compressed_pos
      splatData[i * 4 + 1] = compressed.word[1]; // compressed_scale
      splatData[i * 4 + 2] = compressed.word[2]; // compressed_quat
      splatData[i * 4 + 3] = compressed.word[3]; // compressed_color
    }

    // Upload compressed texture (RGBA32F, single layer)
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, compressedSplatsTexture_);
    glContext->texStorage2D(WebGLTexture2DTarget::kTexture2D, 1, WEBGL2_RGBA32F, width, height);

    // Upload texture data
    glContext->texSubImage2D(WebGLTexture2DTarget::kTexture2D, 0, 0, 0, width, height, WebGLTextureFormat::kRGBA, WebGLPixelType::kFloat, (unsigned char *)splatData.data());

    // Set texture parameters (nearest sampling for discrete data)
    glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                             WebGLTextureParameterName::kTextureMinFilter,
                             WEBGL_NEAREST);
    glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                             WebGLTextureParameterName::kTextureMagFilter,
                             WEBGL_NEAREST);
    glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                             WebGLTextureParameterName::kTextureWrapS,
                             WEBGL_CLAMP_TO_EDGE);
    glContext->texParameteri(WebGLTextureTarget::kTexture2D,
                             WebGLTextureParameterName::kTextureWrapT,
                             WEBGL_CLAMP_TO_EDGE);

    // Reset the flag since textures have been updated
    needsTextureUpdate_ = false;

    DEBUG("GaussianSplatsMesh", "Updated compressed splat texture: %zu splats, %ux%u texture2D", compressedSplatData_.size(), width, height);
  }

  void GaussianSplatsMesh::updateSplatTexturesIfNeeded()
  {
    if (needsTextureUpdate_)
    {
      auto glContext = glContext_.lock();
      if (glContext)
      {
        updateSplatTextures(glContext);
      }
    }
  }

  void GaussianSplatsMesh::onMesh3dInitialized(const Mesh3d &mesh3d,
                                               shared_ptr<WebGL2Context> glContext)
  {
    // Call parent implementation first
    Mesh::onMesh3dInitialized(mesh3d, glContext);

    // Store the glContext for iterateInstanceAttributes
    glContext_ = glContext;

    // Initialize the splat buffer
    setupSplatBuffer(glContext, mesh3d.vertexArrayObject());
  }

  size_t GaussianSplatsMesh::iterateInstanceAttributes(shared_ptr<WebGLProgram> program,
                                                       function<void(const IVertexAttribute &,
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
        unique_ptr<IVertexAttribute> attrib = nullptr;

        // Configure based on attribute name and type
        if (name == "splatIndex")
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
