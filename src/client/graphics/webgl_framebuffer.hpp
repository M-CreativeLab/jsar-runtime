#pragma once

#include <map>
#include "common/command_buffers/details/program.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLFramebuffer : public WebGLObject
  {
  public:
    WebGLFramebuffer() : WebGLObject(WebGLObjectType::Framebuffer) {}
  };
}
