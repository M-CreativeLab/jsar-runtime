#include <cstring>
#include <command_buffers/shared.hpp>
#include <command_buffers/webgl_constants.hpp>
#include <command_buffers/gpu/physical_device.hpp>
#include <renderer/context_webgl.hpp>
#include <renderer/content_renderer.hpp>
#include <renderer/renderer.hpp>

namespace renderer
{
  using namespace std;
  using namespace commandbuffers;

  void TrContextWebGL::glGetInternalformativ(WebGLenum target, WebGLenum internalformat, WebGLsizei propCount, WebGLenum *props, WebGLint *params)
  {
    if (propCount <= 0 || props == nullptr || params == nullptr)
      return;
    for (WebGLsizei i = 0; i < propCount; i++)
    {
      WebGLenum pname = props[i];
      if (pname == WEBGL_SAMPLES)
      {
        params[i] = 1;
      }
      else if (pname == WEBGL2_MAX_SAMPLES)
      {
        params[i] = 1;
      }
      else
      {
        params[i] = 0;
      }
    }
  }

  void TrContextWebGL::glGetString(WebGLenum pname, WebGLchar *params)
  {
    if (params == nullptr)
      return;
    auto rhi = content_renderer_->getRendererRef().getRHI();
    string value = "";
    if (rhi != nullptr)
    {
      auto device = rhi->GetGPUDevice();
      if (device != nullptr)
      {
        auto &info = device->adapterInfo();
        auto phys = device->getPhysicalDevice();
        switch (pname)
        {
        case WEBGL_VENDOR:
          value = info.vendor.empty() ? (phys ? phys->vendorName() : "") : info.vendor;
          break;
        case WEBGL_RENDERER:
          value = info.device.empty() ? (phys ? phys->name() : "") : info.device;
          break;
        case WEBGL_VERSION:
          value = phys ? phys->driverDescription() : "";
          break;
        case WEBGL_SHADING_LANGUAGE_VERSION:
          value = "";
          break;
        default:
          value = "";
          break;
        }
      }
    }
    std::strcpy(params, value.c_str());
  }

  void TrContextWebGL::glGetStringi(WebGLenum pname, WebGLuint index)
  {
    (void)pname;
    (void)index;
  }
}
