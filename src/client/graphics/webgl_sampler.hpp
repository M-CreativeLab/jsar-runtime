#pragma once

#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLSampler : public WebGLObject
  {
  public:
    WebGLSampler() : WebGLObject(WebGLObjectType::Sampler) {}
  };
}
