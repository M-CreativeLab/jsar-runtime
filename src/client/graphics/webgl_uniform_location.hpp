#pragma once

#include <string>

namespace client_graphics
{
  class WebGLUniformLocation
  {
  public:
    WebGLUniformLocation(int index, const std::string &name) : index(index), name(name) {}

  public:
    uint32_t index;
    std::string name;
  };
}
