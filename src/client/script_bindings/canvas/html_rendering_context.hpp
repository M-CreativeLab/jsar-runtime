#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/html_rendering_context.hpp>

namespace script_bindings
{
  namespace canvas
  {
    class HTMLRenderingContext;
    using HTMLRenderingContextBase = scripting_base::ObjectWrap<HTMLRenderingContext, ::canvas::HTMLRenderingContext>;

    /**
     * HTMLRenderingContext wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::HTMLRenderingContext objects for use in V8 JavaScript execution contexts.
     * It provides the standard HTML rendering context interface for canvas elements.
     */
    class HTMLRenderingContext : public HTMLRenderingContextBase
    {
      using HTMLRenderingContextBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLRenderingContext class for V8.
       */
      static std::string Name()
      {
        return "HTMLRenderingContext";
      }

      /**
       * Configure the V8 function template with HTMLRenderingContext properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 HTMLRenderingContext instance from a native canvas::HTMLRenderingContext.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::HTMLRenderingContext> nativeContext);

      /**
       * Initialize the HTMLRenderingContext class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      HTMLRenderingContext(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void CanvasGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void GetContextAttributes(const v8::FunctionCallbackInfo<v8::Value> &info);
      static void IsContextLost(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}