#include <memory>
#include <vector>
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>
#include <client/graphics/webgl_buffer.hpp>
#include "./gaussian_splatting.hpp"
#include "../meshes.hpp"

namespace builtin_scene::materials
{
  bool GaussianSplattingMaterial::initialize(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                             std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (TR_UNLIKELY(!Material::initialize(glContext, program)))
      return false;

    // Initialize WebGL buffers for instanced gaussian splatting data
    splatPositionBuffer_ = glContext->createBuffer();
    splatColorBuffer_ = glContext->createBuffer();
    splatOpacityBuffer_ = glContext->createBuffer();
    splatScaleBuffer_ = glContext->createBuffer();
    splatRotationBuffer_ = glContext->createBuffer();

    buffersInitialized_ = true;
    return true;
  }

  void GaussianSplattingMaterial::drawMeshImpl(shared_ptr<client_graphics::WebGLProgram> program,
                                               const Mesh3d &,
                                               RenderPass,
                                               optional<XRRenderTarget>)
  {
    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Only handle drawing if we have splats to render
    if (splatInstances_.empty() || !buffersInitialized_)
      return;

    // Do the instanced draw call
    glContext->drawElementsInstanced(
      client_graphics::WebGLDrawMode::kTriangles,
      6, // 6 indices for quad (2 triangles)
      WEBGL_UNSIGNED_INT,
      0,
      splatInstances_.size());
  }

  void GaussianSplattingMaterial::onBeforeDrawMesh(shared_ptr<client_graphics::WebGLProgram> program,
                                                   shared_ptr<Mesh3d> mesh)
  {
    Material::onBeforeDrawMesh(program, mesh);

    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Get the GaussianSplatsMesh from the Mesh3d component
    if (mesh != nullptr)
    {
      auto splatsMesh = mesh->getHandleAs<GaussianSplatsMesh>();
      if (splatsMesh != nullptr)
      {
        // Update splat instances from the mesh
        updateSplatInstances(splatsMesh->getSplatInstances());

        // Setup instanced vertex attributes for rendering
        if (!splatInstances_.empty() && buffersInitialized_)
        {
          setupInstancedAttributes(program, glContext);
        }
      }
    }
  }

  void GaussianSplattingMaterial::setupInstancedAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                           std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {

    // Set up instanced vertex attributes (per splat)
    auto splatPositionLoc = glContext->getAttribLocation(program, "splatPosition");
    if (splatPositionLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatPositionBuffer_);
      glContext->enableVertexAttribArray(splatPositionLoc.value());
      glContext->vertexAttribPointer(splatPositionLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatPositionLoc.value(), 1); // Per instance
    }

    auto splatColorLoc = glContext->getAttribLocation(program, "splatColor");
    if (splatColorLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatColorBuffer_);
      glContext->enableVertexAttribArray(splatColorLoc.value());
      glContext->vertexAttribPointer(splatColorLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatColorLoc.value(), 1); // Per instance
    }

    auto splatOpacityLoc = glContext->getAttribLocation(program, "splatOpacity");
    if (splatOpacityLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatOpacityBuffer_);
      glContext->enableVertexAttribArray(splatOpacityLoc.value());
      glContext->vertexAttribPointer(splatOpacityLoc.value(), 1, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatOpacityLoc.value(), 1); // Per instance
    }

    auto splatScaleLoc = glContext->getAttribLocation(program, "splatScale");
    if (splatScaleLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatScaleBuffer_);
      glContext->enableVertexAttribArray(splatScaleLoc.value());
      glContext->vertexAttribPointer(splatScaleLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatScaleLoc.value(), 1); // Per instance
    }

    auto splatRotationLoc = glContext->getAttribLocation(program, "splatRotation");
    if (splatRotationLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatRotationBuffer_);
      glContext->enableVertexAttribArray(splatRotationLoc.value());
      glContext->vertexAttribPointer(splatRotationLoc.value(), 4, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatRotationLoc.value(), 1); // Per instance
    }

    // Set the quad size uniform
    auto quadSizeLoc = glContext->getUniformLocation(program, "quadSize");
    if (quadSizeLoc.has_value())
    {
      glContext->uniform2f(quadSizeLoc.value(), 1.0f, 1.0f); // Base size, scaled by splat scale
    }
  }

  void GaussianSplattingMaterial::onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                  std::shared_ptr<Mesh3d> mesh)
  {
    auto glContext = glContext_.lock();
    if (glContext)
    {
      auto splatPositionLoc = glContext->getAttribLocation(program, "splatPosition");
      if (splatPositionLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatPositionLoc.value());
        glContext->vertexAttribDivisor(splatPositionLoc.value(), 0);
      }

      auto splatColorLoc = glContext->getAttribLocation(program, "splatColor");
      if (splatColorLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatColorLoc.value());
        glContext->vertexAttribDivisor(splatColorLoc.value(), 0);
      }

      auto splatOpacityLoc = glContext->getAttribLocation(program, "splatOpacity");
      if (splatOpacityLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatOpacityLoc.value());
        glContext->vertexAttribDivisor(splatOpacityLoc.value(), 0);
      }

      auto splatScaleLoc = glContext->getAttribLocation(program, "splatScale");
      if (splatScaleLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatScaleLoc.value());
        glContext->vertexAttribDivisor(splatScaleLoc.value(), 0);
      }

      auto splatRotationLoc = glContext->getAttribLocation(program, "splatRotation");
      if (splatRotationLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatRotationLoc.value());
        glContext->vertexAttribDivisor(splatRotationLoc.value(), 0);
      }
    }

    Material::onAfterDrawMesh(program, mesh);
  }

  void GaussianSplattingMaterial::updateSplatInstances(const std::vector<SplatInstanceData> &instances)
  {
    splatInstances_ = instances;

    if (!buffersInitialized_ || splatInstances_.empty())
      return;

    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Prepare data arrays for GPU upload
    std::vector<float> positions, colors, opacities, scales, rotations;
    positions.reserve(splatInstances_.size() * 3);
    colors.reserve(splatInstances_.size() * 3);
    opacities.reserve(splatInstances_.size());
    scales.reserve(splatInstances_.size() * 3);
    rotations.reserve(splatInstances_.size() * 4);

    for (const auto &instance : splatInstances_)
    {
      // Position (x, y, z)
      positions.push_back(instance.position.x);
      positions.push_back(instance.position.y);
      positions.push_back(instance.position.z);

      // Color (r, g, b)
      colors.push_back(instance.color.r);
      colors.push_back(instance.color.g);
      colors.push_back(instance.color.b);

      // Opacity
      opacities.push_back(instance.opacity);

      // Scale (sx, sy, sz)
      scales.push_back(instance.scale.x);
      scales.push_back(instance.scale.y);
      scales.push_back(instance.scale.z);

      // Rotation quaternion (x, y, z, w)
      rotations.push_back(instance.rotation.x);
      rotations.push_back(instance.rotation.y);
      rotations.push_back(instance.rotation.z);
      rotations.push_back(instance.rotation.w);
    }

    // Upload position data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatPositionBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          positions.size() * sizeof(float),
                          positions.data(),
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Upload color data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatColorBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          colors.size() * sizeof(float),
                          colors.data(),
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Upload opacity data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatOpacityBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          opacities.size() * sizeof(float),
                          opacities.data(),
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Upload scale data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatScaleBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          scales.size() * sizeof(float),
                          scales.data(),
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Upload rotation data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatRotationBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          rotations.size() * sizeof(float),
                          rotations.data(),
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Unbind buffer
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, nullptr);
  }
}