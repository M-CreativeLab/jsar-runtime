#pragma once

#include <string>
#include <optional>

namespace client_graphics
{
  class WebGLAttribLocation
  {
  public:
    WebGLAttribLocation()
        : programId(0)
        , index(std::nullopt)
        , name("")
    {
    }
    WebGLAttribLocation(int programId, const std::string &name)
        : programId(programId)
        , index(std::nullopt)
        , name(name)
    {
    }
    WebGLAttribLocation(int programId, uint32_t index, const std::string &name)
        : programId(programId)
        , index(index)
        , name(name)
    {
    }

  public:
    // The id of the program this uniform belongs to.
    int programId;
    std::string name;
    std::optional<uint32_t> index;
  };
}
