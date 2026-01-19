#pragma once

#include <cassert>
#include <memory>
#include <atomic>
#include <vector>
#include <string_view>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/error.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_adapter.hpp>
#include <common/command_buffers/gpu/gpu_instance.hpp>
#include <common/command_buffers/gpu/gpu_queue.hpp>
#include <common/command_buffers/gpu/gpu_command_buffer.hpp>
#include <common/command_buffers/gpu/gpu_command_encoder.hpp>
#include <common/command_buffers/gpu/gpu_bind_group.hpp>
#include <common/command_buffers/gpu/gpu_buffer.hpp>
#include <common/command_buffers/gpu/gpu_shader_module.hpp>
#include <common/command_buffers/gpu/gpu_texture.hpp>
#include <common/command_buffers/gpu/gpu_texture_view.hpp>
#include <common/command_buffers/gpu/gpu_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_render_pipeline.hpp>
#include <common/command_buffers/gpu/gpu_compute_pipeline.hpp>

namespace commandbuffers
{
  namespace gpu
  {
    class PhysicalDeviceBase;
  }

  struct GPUDeviceDescriptor
  {
    std::string_view label;
    size_t requiredFeatureCount = 0;
    GPUFeatureName const *requiredFeatures = nullptr;
    GPUSupportedLimits *requiredLimits = nullptr;
  };

  class GPUDeviceBase : public std::enable_shared_from_this<GPUDeviceBase>
  {
  public:
    GPUDeviceBase(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor);
    virtual ~GPUDeviceBase() = default;

  public:
    const GPUAdapterInfo &adapterInfo() const;
    const GPUSupportedFeatures &features() const;
    const GPUSupportedLimits &limits() const;

    bool validateHandle(const GPUHandle &handle) const;

    GPUInstance *getInstance() const;
    GPUAdapterBase *getAdapter() const;
    gpu::PhysicalDeviceBase *getPhysicalDevice() const;

    virtual std::unique_ptr<GPUCommandBufferBase> createCommandBuffer(
      GPUCommandEncoder &encoder,
      const GPUCommandBufferDescriptor *descriptor = nullptr) = 0;

    Ref<GPUBindGroupLayoutBase> getOrCreateBindGroupLayout(
      const GPUBindGroupLayoutDescriptor &descriptor);

    GPUBindGroupLayoutBase *getEmptyBindGroupLayout();
    Ref<GPUPipelineLayoutBase> getEmptyPipelineLayout();

    // Object creation methods that be used in a reentrant manner.
    Ref<GPUBindGroupBase> createBindGroup(const GPUBindGroupDescriptor *,
                                          GPUUsageValidationMode mode = GPUUsageValidationMode::kDefault);
    Ref<GPUBindGroupLayoutBase> createBindGroupLayout(const GPUBindGroupLayoutDescriptor *,
                                                      bool allow_internal_binding = false);
    Ref<GPUBufferBase> createBuffer(const GPUBufferDescriptor *);
    Ref<GPUCommandEncoder> createCommandEncoder(const GPUCommandEncoderDescriptor *);
    Ref<GPUComputePipelineBase> createComputePipeline(const GPUComputePipelineDescriptor *);
    Ref<GPURenderPipelineBase> createRenderPipeline(const GPURenderPipelineDescriptor *);
    Ref<GPUShaderModuleBase> createShaderModule(const GPUShaderModuleDescriptor *,
                                                const std::vector<wgsl::Extension> &internal_extensions = {});
    Ref<GPUTextureBase> createTexture(const GPUTextureDescriptor *);
    Ref<GPUTextureViewBase> createTextureView(const GPUTextureViewDescriptor *);

    // The device state which is a combination of creation state and loss state.
    //
    //   - BeingCreated: the device didn't finish creation yet and the frontend cannot be used
    //     (both for the application calling WebGPU, or re-entrant calls). No work exists on
    //     the GPU timeline.
    //   - Alive: the device is usable and might have work happening on the GPU timeline.
    //   - BeingDisconnected: the device is no longer usable because we are waiting for all
    //     work on the GPU timeline to finish. (this is to make validation prevent the
    //     application from adding more work during the transition from Available to
    //     Disconnected)
    //   - Disconnected: there is no longer work happening on the GPU timeline and the CPU data
    //     structures can be safely destroyed without additional synchronization.
    //   - Destroyed: the device is disconnected and resources have been reclaimed.
    enum class State
    {
      BeingCreated,
      Alive,
      BeingDisconnected,
      Disconnected,
      Destroyed,
    };
    State getState() const;
    bool isLost() const;

    gpu::MaybeError validateIsAlive() const;
    bool isValidationEnabled() const;
    bool isRobustnessEnabled() const;
    bool isCompatibilityMode() const;
    bool isImmediateErrorHandlingEnabled() const;

    gpu::MaybeError tick();

    virtual bool mayRequireDuplicationOfIndirectParameters() const;

    Ref<GPUQueueBase> queue() const
    {
      return queue_;
    }

  protected:
    void setQueue(Ref<GPUQueueBase> queue)
    {
      queue_ = std::move(queue);
    }
    void transitionToAlive()
    {
      state_ = State::Alive;
    }

  protected:
    GPUAdapterInfo adapter_info_;
    GPUSupportedFeatures features_;
    GPUSupportedLimits limits_;

  private:
    virtual Ref<GPUBindGroupBase> createBindGroupImpl(const GPUBindGroupDescriptor &) = 0;
    virtual Ref<GPUBindGroupLayoutInternalBase> createBindGroupLayoutImpl(const GPUBindGroupLayoutDescriptor &) = 0;
    virtual Ref<GPUBufferBase> createBufferImpl(const GPUBufferDescriptor &) = 0;
    virtual Ref<GPUPipelineLayoutBase> createPipelineLayoutImpl(const GPUPipelineLayoutDescriptor &) = 0;
    virtual Ref<GPUShaderModuleBase> createShaderModuleImpl(const GPUShaderModuleDescriptor &,
                                                            const std::vector<wgsl::Extension> &) = 0;
    virtual Ref<GPUTextureBase> createTextureImpl(const GPUTextureDescriptor &) = 0;
    virtual Ref<GPUTextureViewBase> createTextureViewImpl(Ref<GPUTextureBase> texture,
                                                          const GPUTextureViewDescriptor &) = 0;
    virtual Ref<GPUComputePipelineBase> createUninitializedComputePipelineImpl(const GPUComputePipelineDescriptor &) = 0;
    virtual Ref<GPURenderPipelineBase> createUninitializedRenderPipelineImpl(const GPURenderPipelineDescriptor &) = 0;

    virtual bool tickImpl() = 0;
    virtual void setLabelImpl();
    virtual bool reduceMemoryUsageImpl();
    virtual void performIdleTasksImpl();

    Ref<GPUBindGroupLayoutBase> createEmptyBindGroupLayout();
    Ref<GPUPipelineLayoutBase> createEmptyPipelineLayout();

    std::atomic<State> state_ = State::BeingCreated;
    Ref<GPUAdapterBase> adapter_;
    Ref<GPUQueueBase> queue_;

    Ref<GPUBindGroupLayoutBase> empty_bind_group_layout_;
    Ref<GPUPipelineLayoutBase> empty_pipeline_layout_;
  };
}
