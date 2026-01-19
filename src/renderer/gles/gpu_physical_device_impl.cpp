// Copyright 2022 The Dawn & Tint Authors
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <renderer/gles/common.hpp>
#include <renderer/gles/gpu_physical_device_impl.hpp>
#include <renderer/gles/gpu_device_impl.hpp>

using namespace std;
using namespace commandbuffers;

namespace gles
{
  struct Vendor
  {
    const char *vendorName;
    uint32_t vendorId;
  };

  const Vendor kVendors[] = {{"ATI", gpu_info::kVendorID_AMD},
                             {"ARM", gpu_info::kVendorID_ARM},
                             {"Imagination", gpu_info::kVendorID_ImgTec},
                             {"Intel", gpu_info::kVendorID_Intel},
                             {"NVIDIA", gpu_info::kVendorID_Nvidia},
                             {"Qualcomm", gpu_info::kVendorID_QualcommPCI}};

  uint32_t GetVendorIdFromVendors(const char *vendor)
  {
    uint32_t vendorId = 0;
    for (const auto &it : kVendors)
    {
      // Matching vendor name with vendor string
      if (strstr(vendor, it.vendorName) != nullptr)
      {
        vendorId = it.vendorId;
        break;
      }
    }
    return vendorId;
  }

  uint32_t GetDeviceIdFromRender(string_view render)
  {
    uint32_t deviceId = 0;
    size_t pos = render.find("(0x");
    if (pos == string_view::npos)
    {
      pos = render.find("(0X");
    }
    if (pos == string_view::npos)
    {
      return deviceId;
    }
    render.remove_prefix(pos + 3);

    // The first character after the prefix must be hexadecimal, otherwise an invalid argument
    // exception is thrown.
    if (!render.empty() && isxdigit(static_cast<unsigned char>(*render.data())))
    {
      deviceId = static_cast<uint32_t>(stoul(render.data(), nullptr, 16));
    }

    return deviceId;
  }

  // static
  Ref<GPUPhysicalDeviceImpl> GPUPhysicalDeviceImpl::Create(GPUBackendType backendType,
                                                           Ref<DisplayEGL> display,
                                                           bool forceES31AndMinExtensions)
  {
    Ref<GPUPhysicalDeviceImpl> physicalDevice = AcquireRef(new GPUPhysicalDeviceImpl(backendType, display));
    physicalDevice->initialize();
    return physicalDevice;
  }

  GPUPhysicalDeviceImpl::GPUPhysicalDeviceImpl(GPUBackendType backendType, Ref<DisplayEGL> display)
      : PhysicalDeviceBase(backendType)
  {
  }

  DisplayEGL *GPUPhysicalDeviceImpl::display() const
  {
    return display_.get();
  }

  bool GPUPhysicalDeviceImpl::supportsExternalImages() const
  {
    return backendType() == GPUBackendType::kOpenGLES;
  }

  bool GPUPhysicalDeviceImpl::supportsFeatureLevel(GPUFeatureLevel featureLevel,
                                                   GPUInstance *instance) const
  {
    return featureLevel == GPUFeatureLevel::kCompatibility;
  }

  void GPUPhysicalDeviceImpl::initializeImpl()
  {
    name_ = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    vendor_id_ = GetVendorIdFromVendors(vendor);

    driver_description_ = string("OpenGL Version ") +
                          reinterpret_cast<const char *>(glGetString(GL_VERSION));

    if (name_.find("SwiftShader") != std::string::npos)
    {
      adapter_type_ = GPUAdapterType::kCPU;
    }
  }

  void GPUPhysicalDeviceImpl::initializeSupportedFeaturesImpl()
  {
  }

  Ref<GPUDeviceBase> GPUPhysicalDeviceImpl::createDeviceImpl(Ref<GPUAdapterBase> adapter,
                                                             const GPUDeviceDescriptor &descriptor)
  {
    // TODO(yorkie): handle opengl context?
    Ref<GPUDeviceBase> device = GPUDeviceImpl::Create(adapter, descriptor);
    return device;
  }
}
