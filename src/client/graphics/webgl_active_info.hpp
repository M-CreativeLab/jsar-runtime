#pragma once

#include <string>

namespace client_graphics
{
  class WebGLActiveInfo
  {
  public:
    WebGLActiveInfo();

  private:
    std::string name;
    int size;
    int type;
  };
}
