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
    if (!glContext || directSplatData_.empty())
      return;

    // Calculate texture dimensions using direct method
    auto textureSize = direct_splat_utils::getTextureSize(static_cast<uint32_t>(directSplatData_.size()));
    uint32_t width = textureSize[0];  // 1024
    uint32_t height = textureSize[1]; // power of 2 height
    uint32_t maxSplats = textureSize[2];

    // Create textures if not initialized
    if (!textureInitialized_)
    {
      splatCentersTexture_ = glContext->createTexture();
      splatColorsTexture_ = glContext->createTexture();
      splatScalesTexture_ = glContext->createTexture();
      splatQuatTexture_ = glContext->createTexture();

      if (!splatCentersTexture_ || !splatColorsTexture_ || !splatScalesTexture_ || !splatQuatTexture_)
        return;
      textureInitialized_ = true;
    }

    // Prepare texture data arrays
    vector<float> centersData(maxSplats * 3, 0.0f); // RGB for position
    vector<float> colorsData(maxSplats * 4, 0.0f);  // RGBA for color+opacity
    vector<float> scalesData(maxSplats * 3, 0.0f);  // RGB for scale
    vector<float> quatData(maxSplats * 4, 0.0f);    // RGBA for quaternion

    // Fill texture data from direct splats
    for (size_t i = 0; i < directSplatData_.size(); ++i)
    {
      const auto &direct = directSplatData_[i];

      // Centers (RGB)
      centersData[i * 3 + 0] = direct.position[0];
      centersData[i * 3 + 1] = direct.position[1];
      centersData[i * 3 + 2] = direct.position[2];

      // Colors (RGBA)
      colorsData[i * 4 + 0] = direct.color[0];
      colorsData[i * 4 + 1] = direct.color[1];
      colorsData[i * 4 + 2] = direct.color[2];
      colorsData[i * 4 + 3] = direct.color[3];

      // Scales (RGB)
      scalesData[i * 3 + 0] = direct.scale[0];
      scalesData[i * 3 + 1] = direct.scale[1];
      scalesData[i * 3 + 2] = direct.scale[2];

      // Quaternion (RGBA)
      quatData[i * 4 + 0] = direct.quaternion[0];
      quatData[i * 4 + 1] = direct.quaternion[1];
      quatData[i * 4 + 2] = direct.quaternion[2];
      quatData[i * 4 + 3] = direct.quaternion[3];
    }

    // Upload centers texture (RGB32F)
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, splatCentersTexture_);
    glContext->texStorage2D(WebGLTexture2DTarget::kTexture2D, 1, WEBGL2_RGB32F, width, height);
    glContext->texSubImage2D(WebGLTexture2DTarget::kTexture2D, 0, 0, 0, width, height, WebGLTextureFormat::kRGB, WebGLPixelType::kFloat, (unsigned char *)centersData.data());

    // Upload colors texture (RGBA32F)
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, splatColorsTexture_);
    glContext->texStorage2D(WebGLTexture2DTarget::kTexture2D, 1, WEBGL2_RGBA32F, width, height);
    glContext->texSubImage2D(WebGLTexture2DTarget::kTexture2D, 0, 0, 0, width, height, WebGLTextureFormat::kRGBA, WebGLPixelType::kFloat, (unsigned char *)colorsData.data());

    // Upload scales texture (RGB32F)
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, splatScalesTexture_);
    glContext->texStorage2D(WebGLTexture2DTarget::kTexture2D, 1, WEBGL2_RGB32F, width, height);
    glContext->texSubImage2D(WebGLTexture2DTarget::kTexture2D, 0, 0, 0, width, height, WebGLTextureFormat::kRGB, WebGLPixelType::kFloat, (unsigned char *)scalesData.data());

    // Upload quaternion texture (RGBA32F)
    glContext->bindTexture(WebGLTextureTarget::kTexture2D, splatQuatTexture_);
    glContext->texStorage2D(WebGLTexture2DTarget::kTexture2D, 1, WEBGL2_RGBA32F, width, height);
    glContext->texSubImage2D(WebGLTexture2DTarget::kTexture2D, 0, 0, 0, width, height, WebGLTextureFormat::kRGBA, WebGLPixelType::kFloat, (unsigned char *)quatData.data());

    // Set texture parameters for all textures (nearest sampling for discrete data)
    for (auto texture : {splatCentersTexture_, splatColorsTexture_, splatScalesTexture_, splatQuatTexture_})
    {
      glContext->bindTexture(WebGLTextureTarget::kTexture2D, texture);
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
    }

    // Reset the flag since textures have been updated
    needsTextureUpdate_ = false;

    DEBUG("GaussianSplatsMesh", "Updated direct splat textures: %zu splats, %ux%u 2D textures", directSplatData_.size(), width, height);
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

  glm::vec3 GaussianSplatsMesh::extractPositionFromDirect(const DirectSplat &direct) const
  {
    // Direct position extraction - no unpacking needed
    return glm::vec3(direct.position[0], direct.position[1], direct.position[2]);
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
