#pragma once

#include "./webgl_object.hpp"

namespace endor
{
  namespace client_graphics
  {
    class WebGLVertexArray : public WebGLObject
    {
    public:
      WebGLVertexArray()
          : WebGLObject(WebGLObjectType::VertexArray)
      {
      }
    };
  }
} // namespace endor
