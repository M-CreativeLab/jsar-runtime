#pragma once

#include <string>
#include <unordered_map>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace endor
{
  namespace script_bindings::webgl_bindings
  {
    class WebGLExtensions
    {
    public:
      /**
     * Initialize the WebGL extensions system.
     * 
     * @param isolate The V8 isolate.
     */
      static void Initialize(v8::Isolate *isolate);

      /**
     * Check if a WebGL extension is supported at binding level.
     * 
     * @param name The name of the WebGL extension to check.
     * @return true if the extension is supported, false otherwise.
     */
      static bool HasExtension(const std::string &name);

      /**
     * Get the JS object for a WebGL extension.
     * 
     * @param isolate The V8 isolate.
     * @param name The name of the WebGL extension to get.
     * @return The V8 value representing the extension object, or null if not supported.
     */
      static v8::Local<v8::Value> GetExtension(v8::Isolate *isolate, const std::string &name);

    private:
      static std::unordered_map<std::string, v8::Global<v8::Value>> NameToExtensionMap_;
    };
  }
} // namespace endor
