#pragma once

#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace webgl_bindings::extensions
  {
    class OVR_multiview2 : public scripting_base::ObjectWrap<OVR_multiview2>
    {
      using scripting_base::ObjectWrap<OVR_multiview2>::ObjectWrap;
    };
  }
}
