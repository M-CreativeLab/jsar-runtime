#include <common/command_buffers/gpu/gpu_renderpass_encoder.hpp>

namespace commandbuffers
{
  // static
  Ref<GPURenderPassEncoder> GPURenderPassEncoder::Create(const GPURenderPassDescriptor &descriptor,
                                                         GPUCommandEncoder *encoder,
                                                         gpu::EncodingContext *context,
                                                         uint32_t renderTargetWidth,
                                                         uint32_t renderTargetHeight,
                                                         bool depthReadOnly,
                                                         bool stencilReadOnly)
  {
    return AcquireRef(new GPURenderPassEncoder(
      nullptr,
      descriptor,
      encoder,
      context,
      renderTargetWidth,
      renderTargetHeight,
      depthReadOnly,
      stencilReadOnly));
  }
  // static
  Ref<GPURenderPassEncoder> GPURenderPassEncoder::MakeError(GPUCommandEncoder *encoder,
                                                            gpu::EncodingContext *context,
                                                            std::string_view label)
  {
    return AcquireRef(
      new GPURenderPassEncoder(
        nullptr,
        encoder,
        context,
        GPUHandle::kError,
        label));
  }

  GPURenderPassEncoder::GPURenderPassEncoder(Ref<GPUDeviceBase> device,
                                             const GPURenderPassDescriptor &descriptor,
                                             GPUCommandEncoder *commandEncoder,
                                             gpu::EncodingContext *encodingContext,
                                             uint32_t renderTargetWidth,
                                             uint32_t renderTargetHeight,
                                             bool depthReadOnly,
                                             bool stencilReadOnly)
      : GPUHandle(device, descriptor.label.value_or("GPURenderPassEncoder"))
  {
  }

  GPURenderPassEncoder::GPURenderPassEncoder(Ref<GPUDeviceBase> device,
                                             GPUCommandEncoder *commandEncoder,
                                             gpu::EncodingContext *encodingContext,
                                             ErrorTag errorTag,
                                             std::string_view label)
      : GPUHandle(device, errorTag, label)
  {
  }

  void GPURenderPassEncoder::draw(uint32_t vertex_count,
                                  uint32_t instance_count,
                                  uint32_t first_vertex,
                                  uint32_t first_instance)
  {
    (void)vertex_count;
    (void)instance_count;
    (void)first_vertex;
    (void)first_instance;
  }

  void GPURenderPassEncoder::drawIndexed(uint32_t index_count,
                                         uint32_t instance_count,
                                         uint32_t first_index,
                                         int32_t base_vertex,
                                         uint32_t first_instance)
  {
    (void)index_count;
    (void)instance_count;
    (void)first_index;
    (void)base_vertex;
    (void)first_instance;
  }

  void GPURenderPassEncoder::setViewport(float x,
                                         float y,
                                         float width,
                                         float height,
                                         float min_depth,
                                         float max_depth)
  {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    (void)min_depth;
    (void)max_depth;
  }

  void GPURenderPassEncoder::setScissorRect(float x, float y, float width, float height)
  {
    (void)x;
    (void)y;
    (void)width;
    (void)height;
  }

  void GPURenderPassEncoder::setPipeline(const GPURenderPipelineBase &pipeline)
  {
    (void)pipeline;
  }

  void GPURenderPassEncoder::setIndexBuffer(const GPUBufferBase &buffer,
                                            GPUIndexFormat index_format,
                                            uint32_t offset,
                                            uint32_t size)
  {
    (void)buffer;
    (void)index_format;
    (void)offset;
    (void)size;
  }

  void GPURenderPassEncoder::setVertexBuffer(uint32_t slot,
                                             const GPUBufferBase &buffer,
                                             uint32_t offset,
                                             uint32_t size)
  {
    (void)slot;
    (void)buffer;
    (void)offset;
    (void)size;
  }

  void GPURenderPassEncoder::setBlendConstant(float r, float g, float b, float a)
  {
    (void)r;
    (void)g;
    (void)b;
    (void)a;
  }

  void GPURenderPassEncoder::setStencilReference(uint32_t ref)
  {
    (void)ref;
  }
 }
