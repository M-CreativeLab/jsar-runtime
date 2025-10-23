#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    template <typename T>
    class WebGLExtension : public scripting_base::ObjectWrap<T>
    {
      using scripting_base::ObjectWrap<T>::ObjectWrap;
    };
  }
}
