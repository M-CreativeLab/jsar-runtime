#pragma once

#include <memory>
#include <client/scripting_base/v8_object_wrap.hpp>
#include <client/canvas/image_data.hpp>

namespace endor
{
  namespace script_bindings
  {
    namespace canvas_bindings
    {
      class ImageData;
      using ImageDataBase = scripting_base::ObjectWrap<ImageData, ::endor::canvas::ImageData>;

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
        static std::string Name()
        {
          return "ImageData";
        }

        static void ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                                 std::shared_ptr<::endor::canvas::ImageData> nativeImageData);
        static v8::Local<v8::Object> NewInstance(v8::Isolate *isolate,
                                                 int width,
                                                 int height,
                                                 const std::string &colorSpace = "srgb");

      public:
        ImageData(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args);

      private:
        // Property getters
        void WidthGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void HeightGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void DataGetter(const v8::PropertyCallbackInfo<v8::Value> &info);
        void ColorSpaceGetter(const v8::PropertyCallbackInfo<v8::Value> &info);

      private:
        v8::Global<v8::Value> data_value_handle_;
      };
    }
  }
} // namespace endor
