#pragma once

#include <optional>
#include <vector>
#include <variant>

#include <common/utility.hpp>
#include <common/command_buffers/gpu/gpu_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_base.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout.hpp>
#include <common/command_buffers/gpu/gpu_bind_group_layout_base.hpp>

namespace commandbuffers
{
  class GPUDeviceBase;

  class GPUBindGroupBase : public GPUHandle
  {
  public:
    virtual ~GPUBindGroupBase() = default;

  protected:
    GPUBindGroupBase(Ref<GPUDeviceBase> device,
                     const GPUBindGroupDescriptor &descriptor,
                     void *bindingDataStart);

  private:
    GPUBindGroupBase(Ref<GPUDeviceBase> device, GPUHandle::ErrorTag tag, std::string_view label);

    Ref<GPUBindGroupLayoutBase> *layout_ = nullptr;
  };
}
