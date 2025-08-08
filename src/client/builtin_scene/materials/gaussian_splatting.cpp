#include "./gaussian_splatting.hpp"
#include <client/graphics/webgl_context.hpp>
#include <client/graphics/webgl_program.hpp>
#include <client/graphics/webgl_buffer.hpp>
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

    // Initialize base quad geometry buffers
    quadVertexBuffer_ = glContext->createBuffer();
    quadIndexBuffer_ = glContext->createBuffer();

    // Create the base quad geometry
    createQuadGeometry(glContext);

    buffersInitialized_ = true;
    return true;
  }

  void GaussianSplattingMaterial::createQuadGeometry(std::shared_ptr<client_graphics::WebGL2Context> glContext)
  {
    // Create a simple quad (-0.5 to 0.5) for each splat
    // This will be instanced for each gaussian splat
    float vertices[] = {
      // Position (x, y)
      -0.5f,
      -0.5f, // Bottom-left
      0.5f,
      -0.5f, // Bottom-right
      0.5f,
      0.5f, // Top-right
      -0.5f,
      0.5f // Top-left
    };

    unsigned int indices[] = {
      0, 1, 2, // First triangle
      2,
      3,
      0 // Second triangle
    };

    // Upload vertex data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, quadVertexBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kArrayBuffer,
                          sizeof(vertices),
                          vertices,
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    // Upload index data
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, quadIndexBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer,
                          sizeof(indices),
                          indices,
                          client_graphics::WebGLBufferUsage::kStaticDraw);

    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, nullptr);
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, nullptr);

    quadGeometryCreated_ = true;
  }

  void GaussianSplattingMaterial::onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                   std::shared_ptr<Mesh3d> mesh)
  {
    Material::onBeforeDrawMesh(program, mesh);

    if (!buffersInitialized_ || !quadGeometryCreated_ || splats_.empty())
      return;

    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Set up base quad vertex attributes (per vertex)
    auto positionLoc = glContext->getAttribLocation(program, "a_position");
    if (positionLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, quadVertexBuffer_);
      glContext->enableVertexAttribArray(positionLoc.value());
      glContext->vertexAttribPointer(positionLoc.value(), 2, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(positionLoc.value(), 0); // Per vertex, not per instance
    }

    // Set up instanced vertex attributes (per splat)
    auto splatPositionLoc = glContext->getAttribLocation(program, "a_splatPosition");
    if (splatPositionLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatPositionBuffer_);
      glContext->enableVertexAttribArray(splatPositionLoc.value());
      glContext->vertexAttribPointer(splatPositionLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatPositionLoc.value(), 1); // Per instance
    }

    auto splatColorLoc = glContext->getAttribLocation(program, "a_splatColor");
    if (splatColorLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatColorBuffer_);
      glContext->enableVertexAttribArray(splatColorLoc.value());
      glContext->vertexAttribPointer(splatColorLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatColorLoc.value(), 1); // Per instance
    }

    auto splatOpacityLoc = glContext->getAttribLocation(program, "a_splatOpacity");
    if (splatOpacityLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatOpacityBuffer_);
      glContext->enableVertexAttribArray(splatOpacityLoc.value());
      glContext->vertexAttribPointer(splatOpacityLoc.value(), 1, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatOpacityLoc.value(), 1); // Per instance
    }

    auto splatScaleLoc = glContext->getAttribLocation(program, "a_splatScale");
    if (splatScaleLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatScaleBuffer_);
      glContext->enableVertexAttribArray(splatScaleLoc.value());
      glContext->vertexAttribPointer(splatScaleLoc.value(), 3, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatScaleLoc.value(), 1); // Per instance
    }

    auto splatRotationLoc = glContext->getAttribLocation(program, "a_splatRotation");
    if (splatRotationLoc.has_value())
    {
      glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kArrayBuffer, splatRotationBuffer_);
      glContext->enableVertexAttribArray(splatRotationLoc.value());
      glContext->vertexAttribPointer(splatRotationLoc.value(), 4, WEBGL_FLOAT, false, 0, 0);
      glContext->vertexAttribDivisor(splatRotationLoc.value(), 1); // Per instance
    }

    // Set the quad size uniform
    auto quadSizeLoc = glContext->getUniformLocation(program, "u_quadSize");
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
      // Disable vertex attributes and reset divisors
      auto positionLoc = glContext->getAttribLocation(program, "a_position");
      if (positionLoc.has_value())
      {
        glContext->disableVertexAttribArray(positionLoc.value());
        glContext->vertexAttribDivisor(positionLoc.value(), 0);
      }

      auto splatPositionLoc = glContext->getAttribLocation(program, "a_splatPosition");
      if (splatPositionLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatPositionLoc.value());
        glContext->vertexAttribDivisor(splatPositionLoc.value(), 0);
      }

      auto splatColorLoc = glContext->getAttribLocation(program, "a_splatColor");
      if (splatColorLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatColorLoc.value());
        glContext->vertexAttribDivisor(splatColorLoc.value(), 0);
      }

      auto splatOpacityLoc = glContext->getAttribLocation(program, "a_splatOpacity");
      if (splatOpacityLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatOpacityLoc.value());
        glContext->vertexAttribDivisor(splatOpacityLoc.value(), 0);
      }

      auto splatScaleLoc = glContext->getAttribLocation(program, "a_splatScale");
      if (splatScaleLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatScaleLoc.value());
        glContext->vertexAttribDivisor(splatScaleLoc.value(), 0);
      }

      auto splatRotationLoc = glContext->getAttribLocation(program, "a_splatRotation");
      if (splatRotationLoc.has_value())
      {
        glContext->disableVertexAttribArray(splatRotationLoc.value());
        glContext->vertexAttribDivisor(splatRotationLoc.value(), 0);
      }
    }

    Material::onAfterDrawMesh(program, mesh);
  }

  void GaussianSplattingMaterial::updateSplats(const std::vector<GaussianSplat> &splats)
  {
    splats_ = splats;

    if (!buffersInitialized_ || splats_.empty())
      return;

    auto glContext = glContext_.lock();
    if (!glContext)
      return;

    // Prepare data arrays for GPU upload
    std::vector<float> positions, colors, opacities, scales, rotations;
    positions.reserve(splats_.size() * 3);
    colors.reserve(splats_.size() * 3);
    opacities.reserve(splats_.size());
    scales.reserve(splats_.size() * 3);
    rotations.reserve(splats_.size() * 4);

    for (const auto &splat : splats_)
    {
      // Position (x, y, z)
      positions.push_back(splat.position.x);
      positions.push_back(splat.position.y);
      positions.push_back(splat.position.z);

      // Color (r, g, b)
      colors.push_back(splat.color.r);
      colors.push_back(splat.color.g);
      colors.push_back(splat.color.b);

      // Opacity
      opacities.push_back(splat.opacity);

      // Scale (sx, sy, sz)
      scales.push_back(splat.scale.x);
      scales.push_back(splat.scale.y);
      scales.push_back(splat.scale.z);

      // Rotation quaternion (x, y, z, w)
      rotations.push_back(splat.rotation.x);
      rotations.push_back(splat.rotation.y);
      rotations.push_back(splat.rotation.z);
      rotations.push_back(splat.rotation.w);
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

  void GaussianSplattingMaterial::drawInstanced(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                                                std::shared_ptr<client_graphics::WebGLProgram> program)
  {
    if (!buffersInitialized_ || !quadGeometryCreated_ || splats_.empty())
      return;

    // Bind the index buffer for the quad
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, quadIndexBuffer_);

    // Draw instanced quads - one for each splat
    glContext->drawElementsInstanced(
      client_graphics::WebGLDrawMode::kTriangles, // Draw mode
      6,                                          // Number of indices per quad
      WEBGL_UNSIGNED_INT,                         // Index type
      0,                                          // Offset
      static_cast<int>(splats_.size())            // Instance count
    );

    // Unbind buffers
    glContext->bindBuffer(client_graphics::WebGLBufferBindingTarget::kElementArrayBuffer, nullptr);
  }
}