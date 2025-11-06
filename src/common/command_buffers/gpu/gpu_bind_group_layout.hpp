#pragma once

#include <string_view>
#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_texture.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout_internal.hpp>

namespace commandbuffers
{
  class GPUBindGroupLayoutBase : public GPUHandle
  {
  public:
    GPUBindGroupLayoutBase(Ref<GPUDeviceBase> device,
                           std::string_view label,
                           Ref<GPUBindGroupLayoutInternalBase> internal);
    virtual ~GPUBindGroupLayoutBase() = default;

    GPUHandleType type() const override
    {
      return GPUHandleType::kBindGroupLayout;
    }

    GPUBindGroupLayoutInternalBase *getInternalBindGroupLayout() const;
    bool equal(const GPUBindGroupLayoutBase *other,
               bool excludePipelineCompatibiltyToken = false) const;

    bool isEmpty() const;

  private:
    GPUBindGroupLayoutBase(Ref<GPUDeviceBase> device,
                           GPUHandle::ErrorTag tag,
                           std::string_view label);

  private:
    Ref<GPUBindGroupLayoutInternalBase> internal_layout_;
  };
}
