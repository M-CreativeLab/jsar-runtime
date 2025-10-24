#pragma once

#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace endor
{
  namespace client_graphics
  {
    enum class WebGLRenderbufferBindingTarget : uint32_t
    {
      kRenderbuffer = WEBGL_RENDERBUFFER,
    };

    class WebGLRenderbuffer : public WebGLObject
    {
    public:
      WebGLRenderbuffer()
          : WebGLObject(WebGLObjectType::Renderbuffer)
      {
      }
    };
  }
} // namespace endor
