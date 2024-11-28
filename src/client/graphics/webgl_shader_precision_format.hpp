#pragma once

#include <map>
#include "common/command_buffers/details/properties.hpp"
#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLShaderPrecisionFormat
  {
  public:
    WebGLShaderPrecisionFormat(int rangeMin, int rangeMax, int precision)
        : rangeMin(rangeMin), rangeMax(rangeMax), precision(precision)
    {
    }
    WebGLShaderPrecisionFormat(GetShaderPrecisionFormatCommandBufferResponse& resp)
        : rangeMin(resp.rangeMin), rangeMax(resp.rangeMax), precision(resp.precision)
    {
    }

  public:
    const int rangeMin;
    const int rangeMax;
    const int precision;
  };
}
