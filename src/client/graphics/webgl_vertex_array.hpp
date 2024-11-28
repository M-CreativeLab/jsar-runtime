#pragma once

#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLVertexArray : public WebGLObject
  {
  public:
    WebGLVertexArray() : WebGLObject(WebGLObjectType::VertexArray) {}
  };
}
