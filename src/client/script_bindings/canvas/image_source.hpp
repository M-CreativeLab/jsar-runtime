#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/image_source.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class ImageSource;
    using ImageSourceBase = scripting_base::ObjectWrap<ImageSource, ::canvas::ImageSource>;

    class ImageSource : public ImageSourceBase
    {
    public:
      static std::string Name()
      {
        return "ImageSource";
      }
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

    public:
      ImageSource(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NaturalWidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void NaturalHeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void GetImageData(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}