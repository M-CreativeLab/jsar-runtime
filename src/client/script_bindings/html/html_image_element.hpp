#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/html/html_image_element.hpp>
#include "./html_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    class HTMLImageElement;
    using HTMLImageElementBase = scripting_base::ObjectWrap<HTMLImageElement, dom::HTMLImageElement, HTMLElement>;

    class HTMLImageElement : public HTMLImageElementBase
    {
      using HTMLImageElementBase::ObjectWrap;

    public:
      /**
       * The name of the HTMLImageElement class for V8.
       */
      static std::string Name()
      {
        return "HTMLImageElement";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Function> CreateImageConstructor(v8::Isolate *isolate);

    private:
      static void ImageConstructor(const v8::FunctionCallbackInfo<v8::Value> &info);

    public:
      HTMLImageElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      void CurrentSrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NaturalWidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void NaturalHeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      void SrcGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void SrcSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void WidthSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
      void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
      void HeightSetter(v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info);
    };
  }
}
