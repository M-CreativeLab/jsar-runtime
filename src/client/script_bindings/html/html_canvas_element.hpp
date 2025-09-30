#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/script_bindings/html/html_element.hpp>
#include <client/html/html_canvas_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLCanvasElement;
    using HTMLCanvasElementBase = scripting_base::ObjectWrap<HTMLCanvasElement, dom::HTMLCanvasElement, HTMLElement>;

    /**
     * HTMLCanvasElement wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps dom::HTMLCanvasElement objects for use in V8 JavaScript execution contexts.
     * It provides the HTML Canvas element interface for 2D and WebGL rendering contexts.
     */
    class HTMLCanvasElement : public HTMLCanvasElementBase
    {
      using HTMLCanvasElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLCanvasElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLCanvasElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      HTMLCanvasElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters/setters
      void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void WidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HeightSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);

      // Canvas methods
      void GetContext(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ToDataURL(const v8::FunctionCallbackInfo<v8::Value> &info);
      void ToBlob(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}