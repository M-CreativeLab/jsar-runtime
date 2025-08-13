#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>

#include "./mesh_base.hpp"
#include "./meshes.hpp"
#include "./gaussian_splats_mesh.hpp"
#include "./direct_splats.hpp"
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

    // Prepare texture data arrays for 2 layers
    vector<float> layer0Data(maxSplats * 4, 0.0f); // Texel 0: position.xyz, scale.x
    vector<float> layer1Data(maxSplats * 4, 0.0f); // Texel 1: scale.yz, compressed_quat, compressed_color

    // Fill texture data from compressed splats
    for (size_t i = 0; i < compressedSplatData_.size(); ++i)
    {
      const auto &compressed = compressedSplatData_[i];

      // Layer 0 data (texel0)
      layer0Data[i * 4 + 0] = compressed.texel0[0]; // pos.x
      layer0Data[i * 4 + 1] = compressed.texel0[1]; // pos.y
      layer0Data[i * 4 + 2] = compressed.texel0[2]; // pos.z
      layer0Data[i * 4 + 3] = compressed.texel0[3]; // scale.x

      // Layer 1 data (texel1)
      layer1Data[i * 4 + 0] = compressed.texel1[0]; // scale.y
      layer1Data[i * 4 + 1] = compressed.texel1[1]; // scale.z
      layer1Data[i * 4 + 2] = compressed.texel1[2]; // compressed_quat
      layer1Data[i * 4 + 3] = compressed.texel1[3]; // compressed_color
    }

    // Upload compressed texture array (RGBA32F, 2 layers)
    glContext->bindTexture(WebGLTextureTarget::kTexture2DArray, compressedSplatsTexture_);
    glContext->texStorage3D(WebGLTexture3DTarget::kTexture2DArray, 1, WEBGL2_RGBA32F, width, height, 2);

    // Upload layer 0 (texel0 data)
    glContext->texSubImage3D(WebGLTexture3DTarget::kTexture2DArray, 0, 0, 0, 0, width, height, 1, WebGLTextureFormat::kRGBA, WebGLPixelType::kFloat, (unsigned char *)layer0Data.data());

    // Upload layer 1 (texel1 data)
    glContext->texSubImage3D(WebGLTexture3DTarget::kTexture2DArray, 0, 0, 0, 1, width, height, 1, WebGLTextureFormat::kRGBA, WebGLPixelType::kFloat, (unsigned char *)layer1Data.data());

    // Set texture parameters (nearest sampling for discrete data)
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

    // Reset the flag since textures have been updated
    needsTextureUpdate_ = false;

    DEBUG("GaussianSplatsMesh", "Updated compressed splat texture array: %zu splats, %ux%ux2 texture2DArray", compressedSplatData_.size(), width, height);
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

  glm::vec3 GaussianSplatsMesh::extractPositionFromCompressed(const CompressedSplat &compressed) const
  {
    // Compressed position extraction - texel0 contains position xyz
    return glm::vec3(compressed.texel0[0], compressed.texel0[1], compressed.texel0[2]);
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
