#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./mesh_base.hpp"
#include "./meshes.hpp"
#include "./gaussian_splats_mesh.hpp"
#include "./packed_splats.hpp"

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

  void GaussianSplatsMesh::updatePackedSplatTexture(std::shared_ptr<WebGL2Context> glContext)
  {
    if (!glContext || packedSplatData_.empty())
      return;

    // Create 3D array texture if not initialized
    if (!textureInitialized_)
    {
      packedSplatTexture_ = glContext->createTexture();
      if (!packedSplatTexture_)
        return;
      textureInitialized_ = true;
    }

    // Calculate texture dimensions using SparkJS method
    auto textureSize = packed_splat_utils::getTextureSize(static_cast<uint32_t>(packedSplatData_.size()));
    uint32_t width = textureSize[0];
    uint32_t height = textureSize[1];
    uint32_t depth = textureSize[2];
    uint32_t maxSplats = textureSize[3];

    // Prepare texture data - RGBA32F format (4 float per texel = 1 packed splat)
    // Use float values directly from PackedSplat structure
    vector<float> textureData(maxSplats * 4, 0.0f); // 4 float per splat

    for (size_t i = 0; i < packedSplatData_.size(); ++i)
    {
      const auto &packed = packedSplatData_[i];
      size_t baseIndex = i * 4; // 4 float values per packed splat

      // Use float values directly (no conversion needed)
      textureData[baseIndex + 0] = packed.word0;
      textureData[baseIndex + 1] = packed.word1;
      textureData[baseIndex + 2] = packed.word2;
      textureData[baseIndex + 3] = packed.word3;
    }

    // Upload 3D array texture data
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, packedSplatTexture_);

    // Set texture storage
    glContext->texStorage3D(WebGLTexture3DTarget::kTexture2DArray,
                            1,             // levels
                            WEBGL_RGBA32F, // internal format (RGBA32F for compatibility)
                            width,
                            height,
                            depth);

    // Upload texture data
    glContext->texSubImage3D(WebGLTexture3DTarget::kTexture2DArray,
                             0, // level
                             0,
                             0,
                             0, // xoffset, yoffset, zoffset
                             width,
                             height,
                             depth,                     // width, height, depth
                             WebGLTextureFormat::kRGBA, // format (RGBA for float data)
                             WebGLPixelType::kFloat,    // type (float instead of unsigned int)
                             (unsigned char *)textureData.data());

    // Set texture parameters for point sampling (no filtering needed for packed data)
    glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                             WebGLTextureParameterName::kTextureMinFilter,
                             WEBGL_NEAREST);
    glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                             WebGLTextureParameterName::kTextureMagFilter,
                             WEBGL_NEAREST);
    glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                             WebGLTextureParameterName::kTextureWrapS,
                             WEBGL_CLAMP_TO_EDGE);
    glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                             WebGLTextureParameterName::kTextureWrapT,
                             WEBGL_CLAMP_TO_EDGE);
    glContext->texParameteri(WebGLTextureTarget::kTexture2DArray,
                             WebGLTextureParameterName::kTextureWrapR,
                             WEBGL_CLAMP_TO_EDGE);

    // Reset the flag since texture has been updated
    needsTextureUpdate_ = false;

    DEBUG("GaussianSplatsMesh", "Updated packed splat texture: %zu splats, %ux%ux%u array texture", packedSplatData_.size(), width, height, depth);
  }

  void GaussianSplatsMesh::updatePackedSplatTextureIfNeeded()
  {
    if (needsTextureUpdate_)
    {
      auto glContext = glContext_.lock();
      if (glContext)
      {
        updatePackedSplatTexture(glContext);
      }
    }
  }

  glm::vec3 GaussianSplatsMesh::extractPositionFromPacked(const PackedSplat &packed) const
  {
    // Extract position from packed data
    // Word1 contains XY as float16, Word2 contains Z as float16 (low 16 bits)

    // Convert float back to uint32 to access bit patterns
    uint32_t word1_bits = packed_splat_utils::floatToUint32(packed.word1);
    uint32_t word2_bits = packed_splat_utils::floatToUint32(packed.word2);

    // Use utility functions to unpack float16 values
    uint16_t hx = static_cast<uint16_t>(word1_bits & 0xFFFFu);
    uint16_t hy = static_cast<uint16_t>(word1_bits >> 16u);
    uint16_t hz = static_cast<uint16_t>(word2_bits & 0xFFFFu);

    float x = packed_splat_utils::unpackHalf(hx);
    float y = packed_splat_utils::unpackHalf(hy);
    float z = packed_splat_utils::unpackHalf(hz);

    return glm::vec3(x, y, z);
  }

  void GaussianSplatsMesh::onMesh3dInitialized(const Mesh3d &mesh3d,
                                               std::shared_ptr<WebGL2Context> glContext)
  {
    // Call parent implementation first
    Mesh::onMesh3dInitialized(mesh3d, glContext);

    // Store the glContext for iterateInstanceAttributes
    glContext_ = glContext;

    // Initialize the splat buffer
    setupSplatBuffer(glContext, mesh3d.vertexArrayObject());
  }

  size_t GaussianSplatsMesh::iterateInstanceAttributes(std::shared_ptr<WebGLProgram> program,
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
