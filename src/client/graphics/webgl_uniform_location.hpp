#pragma once

#include <string>

namespace client_graphics
{
  class WebGLUniformLocation
  {
  public:
    WebGLUniformLocation()
        : index(0)
        , name("")
    {
    }
    WebGLUniformLocation(uint32_t index, const std::string &name)
        : index(index)
        , name(name)
    {
    }

  public:
    uint32_t index;
    std::string name;
  };
}
