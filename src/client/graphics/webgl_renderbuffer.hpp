#pragma once

#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLRenderbuffer : public WebGLObject
  {
  public:
    WebGLRenderbuffer() : WebGLObject(WebGLObjectType::Renderbuffer) {}
  };
}
