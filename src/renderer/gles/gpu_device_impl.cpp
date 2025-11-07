#include <string>
#include <memory>
#include <sstream>

#include <renderer/gles/common.hpp>
#include <renderer/gles/gpu_device_impl.hpp>

namespace gles
{
  using namespace std;
  using namespace commandbuffers;

  // static
  Ref<GPUDeviceImpl> GPUDeviceImpl::Create(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor)
  {
    Ref<GPUDeviceImpl> device = AcquireRef(new GPUDeviceImpl(adapter, descriptor));
    device->initialize(descriptor);
    return device;
  }

  GPUDeviceImpl::GPUDeviceImpl(Ref<GPUAdapterBase> adapter, const GPUDeviceDescriptor &descriptor)
      : GPUDeviceBase(adapter, descriptor)
  {
    auto get_gl_string = [](GLenum name) -> string
    {
      const char *str = (const char *)glGetString(name);
      return str == nullptr ? "" : string(str);
    };

    // Initialize adapter_info_, features_, and limits_ with appropriate values.
    adapter_info_.architecture = "";
    adapter_info_.vendor = get_gl_string(GL_VENDOR);
    adapter_info_.device = get_gl_string(GL_RENDERER);
    {
      stringstream description_ss;
      description_ss << adapter_info_.vendor << " "
                     << adapter_info_.device << " "
                     << get_gl_string(GL_VERSION) << " "
                     << "GLSL(" << get_gl_string(GL_SHADING_LANGUAGE_VERSION) << ") ";

      description_ss << "Extensions(";
      GLint extensions_len;
      glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_len);
      for (int i = 0; i < extensions_len; i++)
      {
        const GLubyte *s = glGetStringi(GL_EXTENSIONS, i);
        if (s != nullptr)
        {
          description_ss << string((const char *)s);
          if (i < extensions_len - 1)
            description_ss << ", ";
        }
      }
      description_ss << ")";
      adapter_info_.description = description_ss.str();
    }
    DEBUG(LOG_TAG_RENDERER, "GPU Device Info: %s", adapter_info_.toString().c_str());
  }

  bool GPUDeviceImpl::initialize(const GPUDeviceDescriptor &descriptor)
  {
    return true;
  }

  unique_ptr<GPUCommandBufferBase> GPUDeviceImpl::createCommandBuffer(GPUCommandEncoder &encoder,
                                                                      const GPUCommandBufferDescriptor *descriptor)
  {
    return nullptr;
  }

  Ref<GPUBindGroupBase> GPUDeviceImpl::createBindGroupImpl(
    const GPUBindGroupDescriptor &descriptor)
  {
    return nullptr;
  }

  Ref<GPUBindGroupLayoutInternalBase> GPUDeviceImpl::createBindGroupLayoutImpl(
    const GPUBindGroupLayoutDescriptor &descriptor)
  {
    return nullptr;
  }
}
