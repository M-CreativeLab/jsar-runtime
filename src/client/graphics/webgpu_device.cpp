#include "./webgpu_device.hpp"
#include "./webgpu_queue.hpp"
#include "./webgpu_command_encoder.hpp"

#include <iostream>
#include <stdexcept>

namespace client_graphics
{
  using namespace std;

  // WebGPUDevice implementation
  WebGPUDevice::WebGPUDevice(const commandbuffers::GPUAdapterInfo &adapter_info,
                             std::optional<std::string> label)
      : adapter_info_(adapter_info)
      , features_()
      , limits_()
      , queue_(std::make_unique<WebGPUQueue>("DefaultQueue"))
      , label_(label.value_or("WebGPUDevice"))
  {
    // Device initialization would typically involve creating actual GPU resources
    // For this client-side implementation, we just set up the command recording infrastructure
  }

  std::unique_ptr<WebGPUCommandEncoder> WebGPUDevice::createCommandEncoder(
    std::optional<std::string> label)
  {
    if (is_lost_)
    {
      if (error_callback_)
      {
        error_callback_("device-lost", "Cannot create command encoder on lost device");
      }
      return nullptr;
    }

    return std::make_unique<WebGPUCommandEncoder>(label);
  }

  void WebGPUDevice::setUncapturedErrorCallback(
    std::function<void(const std::string &, const std::string &)> callback)
  {
    error_callback_ = std::move(callback);
  }

  void WebGPUDevice::simulateLoss()
  {
    is_lost_ = true;
    if (error_callback_)
    {
      error_callback_("device-lost", "Device lost");
    }
  }
}