#pragma once

#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>

#include "./OES_element_index_uint.hpp"
#include "./OES_standard_derivatives.hpp"
#include "./OES_texture_float_linear.hpp"
#include "./OVR_multiview2.hpp"

namespace script_bindings::webgl_bindings
{
  class WebGLExtensions
  {
  public:
    /**
     * Check if a WebGL extension is supported at binding level.
     * 
     * @param name The name of the WebGL extension to check.
     * @return true if the extension is supported, false otherwise.
     */
    static inline bool HasExtension(const std::string &name)
    {
      return false;
    }

    /**
     * Get the JS object for a WebGL extension.
     * 
     * @param isolate The V8 isolate.
     * @param name The name of the WebGL extension to get.
     * @return The V8 value representing the extension object, or null if not supported.
     */
    static inline v8::Local<v8::Value> GetExtension(v8::Isolate *isolate, const std::string &name)
    {
      v8::EscapableHandleScope scope(isolate);
      v8::Local<v8::Value> extension = v8::Null(isolate);

      return scope.Escape(extension);
    }
  };
}
