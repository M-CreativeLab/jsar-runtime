#pragma once

#include <string>
#include <optional>
#include <client/scripting_base/v8_object_holder.hpp>

namespace endor
{
  namespace client_graphics
  {
    class WebGLUniformLocation : public scripting_base::JSObjectHolder
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
      std::string name;
      std::optional<uint32_t> index;
    };
  }
} // namespace endor
