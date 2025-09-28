#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class HTMLRenderingContext;
    using HTMLRenderingContextBase = scripting_base::ObjectWrap<HTMLRenderingContext>;

    class HTMLRenderingContext : public HTMLRenderingContextBase
    {
      using HTMLRenderingContextBase::ObjectWrap;

    public:
      static std::string Name()
      {
        return "HTMLRenderingContext";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate);

    public:
      HTMLRenderingContext(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void CanvasGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void GetContextAttributes(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}