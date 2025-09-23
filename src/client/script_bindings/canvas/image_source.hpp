#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/image_source.hpp>

namespace script_bindings
{
  namespace canvas
  {
    class ImageSource;
    using ImageSourceBase = scripting_base::ObjectWrap<ImageSource, ::canvas::ImageSource>;

    /**
     * ImageSource wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::ImageSource objects for use in V8 JavaScript execution contexts.
     * It provides the standard ImageSource interface for image loading and manipulation.
     */
    class ImageSource : public ImageSourceBase
    {
      using ImageSourceBase::ObjectWrap;

    public:
      /**
       * The name of the ImageSource class for V8.
       */
      static std::string Name()
      {
        return "ImageSource";
      }

      /**
       * Configure the V8 function template with ImageSource properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 ImageSource instance from a native canvas::ImageSource.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::ImageSource> nativeImageSource);

      /**
       * Initialize the ImageSource class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

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