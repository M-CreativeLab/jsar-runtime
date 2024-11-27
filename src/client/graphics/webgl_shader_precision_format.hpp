#pragma once

#include <map>
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

  public:
    const int rangeMin;
    const int rangeMax;
    const int precision;
  };
}
