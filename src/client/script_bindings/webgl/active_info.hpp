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
      static std::string Name()
      {
        return "WebGLActiveInfo";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               const client_graphics::WebGLActiveInfo &activeInfo);

    public:
      WebGLActiveInfo(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      void NameGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void TypeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void SizeGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
    };

  } // namespace webgl
} // namespace script_bindings