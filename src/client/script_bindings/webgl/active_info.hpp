#pragma once

#include <memory>
#include <string>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/graphics/webgl_active_info.hpp>

namespace script_bindings
{
  namespace webgl_bindings
  {
    class WebGLActiveInfo;
    using WebGLActiveInfoBase = scripting_base::ObjectWrap<WebGLActiveInfo, client_graphics::WebGLActiveInfo>;

    class WebGLActiveInfo : public WebGLActiveInfoBase
    {
      using WebGLActiveInfoBase::ObjectWrap;

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
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<client_graphics::WebGLActiveInfo> nativeInfo);

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