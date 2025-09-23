#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/webgl/webgl_active_info.hpp>

namespace script_bindings
{
  namespace webgl
  {
    /**
     * WebGLActiveInfo V8 binding for shader active attribute and uniform information.
     * 
     * This class wraps webgl::WebGLActiveInfo objects for use in V8 JavaScript execution contexts.
     * It provides information about active attributes and uniforms in shader programs.
     */
    class WebGLActiveInfo : public scripting_base::ObjectWrap<WebGLActiveInfo, webgl::WebGLActiveInfo>
    {
      using ObjectWrap::ObjectWrap;

    public:
      /**
       * The name of the WebGLActiveInfo class for V8.
       */
      static std::string Name()
      {
        return "WebGLActiveInfo";
      }

      /**
       * Configure the V8 function template with WebGLActiveInfo properties.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 WebGLActiveInfo instance from a native webgl::WebGLActiveInfo.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<webgl::WebGLActiveInfo> nativeInfo);

      /**
       * Initialize the WebGLActiveInfo class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      WebGLActiveInfo(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void NameGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void TypeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void SizeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };

  } // namespace webgl
} // namespace script_bindings