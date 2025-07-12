#pragma once

#include <string>
#include <optional>

namespace client_graphics
{
  class WebGLUniformLocation
  {
  public:
    WebGLUniformLocation()
        : programId(0)
        , index(std::nullopt)
        , name("")
    {
    }
    WebGLUniformLocation(int programId, const std::string &name)
        : programId(programId)
        , index(std::nullopt)
        , name(name)
    {
    }
    WebGLUniformLocation(int programId, uint32_t index, const std::string &name)
        : programId(programId)
        , index(index)
        , name(name)
    {
    }

  public:
    // The id of the program this uniform belongs to.
    int programId;
    std::optional<uint32_t> index;
    std::string name;
  };
}
