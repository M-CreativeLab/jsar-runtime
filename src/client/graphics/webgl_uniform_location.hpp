#pragma once

#include <string>

namespace client_graphics
{
  class WebGLUniformLocation
  {
  public:
    WebGLUniformLocation(int index, const std::string &name) : index(index), name(name) {}

  private:
    int index;
    std::string name;
  };
}
