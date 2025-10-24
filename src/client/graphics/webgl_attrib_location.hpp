#pragma once

#include <string>
#include <optional>
#include <ostream>

namespace endor
{
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

      friend std::ostream &operator<<(std::ostream &os, const WebGLAttribLocation &loc)
      {
        os << "WebGLAttribLocation("
           << "name='" << loc.name << "'";
        if (loc.index.has_value())
          os << ", index=" << loc.index.value();
        else
          os << ", index=nullopt";
        os << ")";
        return os;
      }

    public:
      // The id of the program this uniform belongs to.
      int programId;
      std::string name;
      std::optional<uint32_t> index;
    };
  }
} // namespace endor
