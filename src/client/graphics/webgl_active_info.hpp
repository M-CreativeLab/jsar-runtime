#pragma once

#include <string>
#include "common/command_buffers/details/properties.hpp"

namespace client_graphics
{
  class WebGLActiveInfo
  {
  public:
    WebGLActiveInfo()
    {
    }
    WebGLActiveInfo(const commandbuffers::ActiveInfo &info)
        : name(info.name)
        , size(info.size)
        , type(info.type)
    {
    }

  public:
    std::string name;
    int size;
    int type;
  };
}
