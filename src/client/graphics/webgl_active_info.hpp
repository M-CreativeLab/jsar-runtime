#pragma once

#include <string>
#include <common/command_buffers/details/program.hpp>
#include <client/scripting_base/v8_object_holder.hpp>

namespace endor
{
  namespace client_graphics
  {
    class WebGLActiveInfo : public scripting_base::JSObjectHolder
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
      WebGLActiveInfo(const std::string &name, int size, int type)
          : name(name)
          , size(size)
          , type(type)
      {
      }

    public:
      std::string name;
      int size;
      int type;
    };
  }
} // namespace endor
