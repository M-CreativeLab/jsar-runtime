#pragma once

#include <client/script_bindings/webgl/extension_base.hpp>

namespace script_bindings
{
  namespace webgl_bindings::extensions
  {
    class OES_element_index_uint : public WebGLExtension<OES_element_index_uint>
    {
      using WebGLExtension<OES_element_index_uint>::WebGLExtension;

    public:
      static std::string Name()
      {
        return "OES_element_index_uint";
      }
    };
  }
}
