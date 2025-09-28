#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/image_bitmap.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    class ImageBitmap;
    using ImageBitmapBase = scripting_base::ObjectWrap<ImageBitmap, ::canvas::ImageBitmap>;

    /**
     * ImageBitmap wrapper for V8 objects using scripting_base::ObjectWrap.
     * 
     * This class wraps canvas::ImageBitmap objects for use in V8 JavaScript execution contexts.
     * It provides the standard ImageBitmap interface including width, height, and close methods.
     */
    class ImageBitmap : public ImageBitmapBase
    {
      using ImageBitmapBase::ObjectWrap;

    public:
      /**
       * The name of the ImageBitmap class for V8.
       */
      static std::string Name()
      {
        return "ImageBitmap";
      }

      static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
      static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                               std::shared_ptr<::canvas::ImageBitmap> nativeBitmap);

    public:
      ImageBitmap(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

    private:
      // Property getters
      static void WidthGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);
      static void HeightGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info);

      // Methods
      static void Close(const v8::FunctionCallbackInfo<v8::Value> &info);
    };
  }
}