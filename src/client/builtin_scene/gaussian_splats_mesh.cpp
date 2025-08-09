#include <algorithm>
#include <functional>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>
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
    if (!glContext || !bufferInitialized_ || sortedSplats_.empty())
      return;

    // Prepare a contiguous array of GPU data (excluding depth and sourceEntity)
    std::vector<float> gpuData;
    size_t floatsPerSplat = SPLAT_STRIDE / sizeof(float);
    gpuData.reserve(sortedSplats_.size() * floatsPerSplat);

    for (const auto &splat : sortedSplats_)
    {
      // Position (vec3)
      gpuData.push_back(splat.position.x);
      gpuData.push_back(splat.position.y);
      gpuData.push_back(splat.position.z);

      // Color (vec3)
      gpuData.push_back(splat.color.r);
      gpuData.push_back(splat.color.g);
      gpuData.push_back(splat.color.b);

      // Opacity (float)
      gpuData.push_back(splat.opacity);

      // Scale (vec3)
      gpuData.push_back(splat.scale.x);
      gpuData.push_back(splat.scale.y);
      gpuData.push_back(splat.scale.z);

      // Rotation (vec4)
      gpuData.push_back(splat.rotation.x);
      gpuData.push_back(splat.rotation.y);
      gpuData.push_back(splat.rotation.z);
      gpuData.push_back(splat.rotation.w);
    }

    // Upload to GPU
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatInstanceBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          gpuData.size() * sizeof(float),
                          gpuData.data(),
                          client_graphics::WebGLBufferUsage::kDynamicDraw);
  }

  void GaussianSplatsMesh::setupSplatAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    if (!glContext || !bufferInitialized_)
      return;

    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatInstanceBuffer_);

    size_t offset = 0;

    // splatPosition (vec3)
    auto positionLoc = glContext->getAttribLocation(program, "splatPosition");
    if (positionLoc.has_value())
    {
      glContext->enableVertexAttribArray(positionLoc.value());
      glContext->vertexAttribPointer(positionLoc.value(), 3, WEBGL_FLOAT, false, SPLAT_STRIDE, reinterpret_cast<void *>(offset));
      glContext->vertexAttribDivisor(positionLoc.value(), 1);
    }
    offset += sizeof(glm::vec3);

    // splatColor (vec3)
    auto colorLoc = glContext->getAttribLocation(program, "splatColor");
    if (colorLoc.has_value())
    {
      glContext->enableVertexAttribArray(colorLoc.value());
      glContext->vertexAttribPointer(colorLoc.value(), 3, WEBGL_FLOAT, false, SPLAT_STRIDE, reinterpret_cast<void *>(offset));
      glContext->vertexAttribDivisor(colorLoc.value(), 1);
    }
    offset += sizeof(glm::vec3);

    // splatOpacity (float)
    auto opacityLoc = glContext->getAttribLocation(program, "splatOpacity");
    if (opacityLoc.has_value())
    {
      glContext->enableVertexAttribArray(opacityLoc.value());
      glContext->vertexAttribPointer(opacityLoc.value(), 1, WEBGL_FLOAT, false, SPLAT_STRIDE, reinterpret_cast<void *>(offset));
      glContext->vertexAttribDivisor(opacityLoc.value(), 1);
    }
    offset += sizeof(float);

    // splatScale (vec3)
    auto scaleLoc = glContext->getAttribLocation(program, "splatScale");
    if (scaleLoc.has_value())
    {
      glContext->enableVertexAttribArray(scaleLoc.value());
      glContext->vertexAttribPointer(scaleLoc.value(), 3, WEBGL_FLOAT, false, SPLAT_STRIDE, reinterpret_cast<void *>(offset));
      glContext->vertexAttribDivisor(scaleLoc.value(), 1);
    }
    offset += sizeof(glm::vec3);

    // splatRotation (vec4)
    auto rotationLoc = glContext->getAttribLocation(program, "splatRotation");
    if (rotationLoc.has_value())
    {
      glContext->enableVertexAttribArray(rotationLoc.value());
      glContext->vertexAttribPointer(rotationLoc.value(), 4, WEBGL_FLOAT, false, SPLAT_STRIDE, reinterpret_cast<void *>(offset));
      glContext->vertexAttribDivisor(rotationLoc.value(), 1);
    }

    // Set the quad size uniform
    auto quadSizeLoc = glContext->getUniformLocation(program, "quadSize");
    if (quadSizeLoc.has_value())
    {
      glContext->uniform2f(quadSizeLoc.value(), 1.0f, 1.0f);
    }
  }

  void GaussianSplatsMesh::onMesh3dInitialized(const Mesh3d &mesh3d,
                                               std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    // Call parent implementation first
    Mesh::onMesh3dInitialized(mesh3d, glContext);

    // Initialize the splat buffer
    setupSplatBuffer(glContext, nullptr);
  }
}
