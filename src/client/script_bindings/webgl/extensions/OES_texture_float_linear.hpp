#pragma once

#include <client/script_bindings/webgl/extension_base.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace webgl_bindings::extensions
    {
      class OES_texture_float_linear : public WebGLExtension<OES_texture_float_linear>
      {
        using WebGLExtension<OES_texture_float_linear>::WebGLExtension;

      public:
        static std::string Name()
        {
          return "OES_texture_float_linear";
        }
      };
    }
  }
} // namespace endor
