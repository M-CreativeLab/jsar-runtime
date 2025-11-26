#include <renderer/render_resource.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  TrRenderResource::TrRenderResource(Ref<GPUDeviceBase> device)
      : device_(device)
      , pipelines_()
      , buffers_()
      , textures_()
  {
  }

  Ref<GPUShaderModuleBase> TrRenderResource::createShaderModule(
    const GPUShaderModuleDescriptor *descriptor)
  {
    auto shader_module = device_->createShaderModule(descriptor);
    shader_modules_.emplace(shader_module->id, shader_module);
    return shader_module;
  }

  Ref<GPUPipelineBase> TrRenderResource::createPipeline()
  {
    return nullptr;
  }

  Ref<GPUBufferBase> TrRenderResource::createBuffer(
    const GPUBufferDescriptor *descriptor)
  {
    auto buffer = device_->createBuffer(descriptor);
    buffers_.emplace(buffer->id, buffer);
    return buffer;
  }

  Ref<GPUTextureBase> TrRenderResource::createTexture(
    const GPUTextureDescriptor *descriptor)
  {
    return nullptr;
  }

  Ref<GPUShaderModuleBase> TrRenderResource::getShaderModule(int id)
  {
    return shader_modules_.at(id);
  }

  Ref<GPUPipelineBase> TrRenderResource::getPipeline(int id)
  {
    return pipelines_.at(id);
  }

  Ref<GPUBufferBase> TrRenderResource::getBuffer(int id)
  {
    return buffers_.at(id);
  }

  Ref<GPUTextureBase> TrRenderResource::getTexture(int id)
  {
    return textures_.at(id);
  }
}
