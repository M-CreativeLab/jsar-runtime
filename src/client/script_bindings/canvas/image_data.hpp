#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/image_data.hpp>

namespace script_bindings
{
  namespace canvas
  {
    class ImageData;
    using ImageDataBase = scripting_base::ObjectWrap<ImageData, ::canvas::ImageData>;

    /**
     * ImageData wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::ImageData objects for use in V8 JavaScript execution contexts.
     * It provides the standard ImageData interface including width, height, data, and colorSpace properties.
     */
    class ImageData : public ImageDataBase
    {
      using ImageDataBase::ObjectWrap;

    public:
      /**
       * The name of the ImageData class for V8.
       */
      static std::string Name()
      {
        return "ImageData";
      }

      /**
       * Configure the V8 function template with ImageData properties and methods.
       */
      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);

      /**
       * Create a new V8 ImageData instance from a native canvas::ImageData.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, std::shared_ptr<::canvas::ImageData> nativeImageData);

      /**
       * Create a new V8 ImageData instance with the given width, height, and color space.
       */
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate, int width, int height, const std::string &colorSpace = "srgb");

      /**
       * Initialize the ImageData class and register it with V8.
       */
      static v8::Local<v8::Function> Initialize(v8::Isolate *isolate);

    public:
      ImageData(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void DataGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void ColorSpaceGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
    };
  }
}