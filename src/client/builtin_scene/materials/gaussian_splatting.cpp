#include "./gaussian_splatting.hpp"
#include <client/graphics/webgl_2_context.hpp>
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

    // Initialize WebGL buffers for gaussian splatting data
    positionBuffer_ = glContext->createBuffer();
    colorBuffer_ = glContext->createBuffer();
    opacityBuffer_ = glContext->createBuffer();
    scaleBuffer_ = glContext->createBuffer();
    rotationBuffer_ = glContext->createBuffer();
    
    buffersInitialized_ = true;
    return true;
  }

  void GaussianSplattingMaterial::onBeforeDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                   std::shared_ptr<Mesh3d> mesh)
  {
    Material::onBeforeDrawMesh(program, mesh);
    
    if (!buffersInitialized_ || splats_.empty())
      return;

    auto glContext = getGLContext();
    
    // Set up vertex attributes for gaussian splatting
    auto positionLoc = glContext->getAttribLocation(program, "a_position");
    auto colorLoc = glContext->getAttribLocation(program, "a_color");
    auto opacityLoc = glContext->getAttribLocation(program, "a_opacity");
    auto scaleLoc = glContext->getAttribLocation(program, "a_scale");
    auto rotationLoc = glContext->getAttribLocation(program, "a_rotation");
    
    // Bind position buffer
    if (positionLoc.has_value()) {
      glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, positionBuffer_);
      glContext->enableVertexAttribArray(positionLoc.value());
      glContext->vertexAttribPointer(positionLoc.value(), 3, client_graphics::WebGLDataType::kFloat, false, 0, 0);
    }
    
    // Bind color buffer
    if (colorLoc.has_value()) {
      glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, colorBuffer_);
      glContext->enableVertexAttribArray(colorLoc.value());
      glContext->vertexAttribPointer(colorLoc.value(), 3, client_graphics::WebGLDataType::kFloat, false, 0, 0);
    }
    
    // Bind opacity buffer
    if (opacityLoc.has_value()) {
      glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, opacityBuffer_);
      glContext->enableVertexAttribArray(opacityLoc.value());
      glContext->vertexAttribPointer(opacityLoc.value(), 1, client_graphics::WebGLDataType::kFloat, false, 0, 0);
    }
    
    // Bind scale buffer
    if (scaleLoc.has_value()) {
      glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, scaleBuffer_);
      glContext->enableVertexAttribArray(scaleLoc.value());
      glContext->vertexAttribPointer(scaleLoc.value(), 3, client_graphics::WebGLDataType::kFloat, false, 0, 0);
    }
    
    // Bind rotation buffer
    if (rotationLoc.has_value()) {
      glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, rotationBuffer_);
      glContext->enableVertexAttribArray(rotationLoc.value());
      glContext->vertexAttribPointer(rotationLoc.value(), 4, client_graphics::WebGLDataType::kFloat, false, 0, 0);
    }
  }

  void GaussianSplattingMaterial::onAfterDrawMesh(std::shared_ptr<client_graphics::WebGLProgram> program,
                                                  std::shared_ptr<Mesh3d> mesh)
  {
    // Disable vertex attributes
    auto glContext = getGLContext();
    
    auto positionLoc = glContext->getAttribLocation(program, "a_position");
    auto colorLoc = glContext->getAttribLocation(program, "a_color");
    auto opacityLoc = glContext->getAttribLocation(program, "a_opacity");
    auto scaleLoc = glContext->getAttribLocation(program, "a_scale");
    auto rotationLoc = glContext->getAttribLocation(program, "a_rotation");
    
    if (positionLoc.has_value()) {
      glContext->disableVertexAttribArray(positionLoc.value());
    }
    if (colorLoc.has_value()) {
      glContext->disableVertexAttribArray(colorLoc.value());
    }
    if (opacityLoc.has_value()) {
      glContext->disableVertexAttribArray(opacityLoc.value());
    }
    if (scaleLoc.has_value()) {
      glContext->disableVertexAttribArray(scaleLoc.value());
    }
    if (rotationLoc.has_value()) {
      glContext->disableVertexAttribArray(rotationLoc.value());
    }
    
    Material::onAfterDrawMesh(program, mesh);
  }

  void GaussianSplattingMaterial::updateSplats(const std::vector<GaussianSplat> &splats)
  {
    splats_ = splats;
    
    if (!buffersInitialized_ || splats_.empty())
      return;

    auto glContext = getGLContext();
    
    // Prepare data arrays
    std::vector<float> positions, colors, opacities, scales, rotations;
    positions.reserve(splats_.size() * 3);
    colors.reserve(splats_.size() * 3);
    opacities.reserve(splats_.size());
    scales.reserve(splats_.size() * 3);
    rotations.reserve(splats_.size() * 4);
    
    for (const auto &splat : splats_) {
      // Position
      positions.push_back(splat.position.x);
      positions.push_back(splat.position.y);
      positions.push_back(splat.position.z);
      
      // Color
      colors.push_back(splat.color.r);
      colors.push_back(splat.color.g);
      colors.push_back(splat.color.b);
      
      // Opacity
      opacities.push_back(splat.opacity);
      
      // Scale
      scales.push_back(splat.scale.x);
      scales.push_back(splat.scale.y);
      scales.push_back(splat.scale.z);
      
      // Rotation (quaternion)
      rotations.push_back(splat.rotation.x);
      rotations.push_back(splat.rotation.y);
      rotations.push_back(splat.rotation.z);
      rotations.push_back(splat.rotation.w);
    }
    
    // Upload data to GPU buffers
    glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, positionBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferType::kArrayBuffer, 
                         positions.size() * sizeof(float), 
                         positions.data(), 
                         client_graphics::WebGLBufferUsage::kDynamicDraw);
    
    glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, colorBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferType::kArrayBuffer, 
                         colors.size() * sizeof(float), 
                         colors.data(), 
                         client_graphics::WebGLBufferUsage::kDynamicDraw);
    
    glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, opacityBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferType::kArrayBuffer, 
                         opacities.size() * sizeof(float), 
                         opacities.data(), 
                         client_graphics::WebGLBufferUsage::kDynamicDraw);
    
    glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, scaleBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferType::kArrayBuffer, 
                         scales.size() * sizeof(float), 
                         scales.data(), 
                         client_graphics::WebGLBufferUsage::kDynamicDraw);
    
    glContext->bindBuffer(client_graphics::WebGLBufferType::kArrayBuffer, rotationBuffer_);
    glContext->bufferData(client_graphics::WebGLBufferType::kArrayBuffer, 
                         rotations.size() * sizeof(float), 
                         rotations.data(), 
                         client_graphics::WebGLBufferUsage::kDynamicDraw);
  }
}