#pragma once

#include <map>
#include <string>
#include <iostream>
#include <common/command_buffers/details/properties.hpp>

#include "./webgl_object.hpp"

namespace client_graphics
{
  class WebGLShaderPrecisionFormat
  {
  public:
    WebGLShaderPrecisionFormat() = default;
    WebGLShaderPrecisionFormat(int rangeMin, int rangeMax, int precision)
        : rangeMin(rangeMin)
        , rangeMax(rangeMax)
        , precision(precision)
    {
    }
    WebGLShaderPrecisionFormat(commandbuffers::GetShaderPrecisionFormatCommandBufferResponse &resp)
        : rangeMin(resp.rangeMin)
        , rangeMax(resp.rangeMax)
        , precision(resp.precision)
    {
    }

    friend std::ostream &operator<<(std::ostream &os, const WebGLShaderPrecisionFormat &format)
    {
      os << "WebGLShaderPrecisionFormat("
         << "[" << format.rangeMin << ", " << format.rangeMax << "], "
         << format.precision << ")";
      return os;
    }

  public:
    int rangeMin = 0;
    int rangeMax = 0;
    int precision = 0;
  };
}
