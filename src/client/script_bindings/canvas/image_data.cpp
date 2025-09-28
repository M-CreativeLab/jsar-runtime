#include "image_data.hpp"
#include <client/canvas/image_data.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace std;
    using namespace v8;

    void ImageData::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

#define NAME(X) String::NewFromUtf8(isolate, X).ToLocalChecked()
#define DEFINE_READONLY_PROPERTY(STR, GETTER)            \
  prototypeTemplate->SetAccessor(NAME(STR),              \
                                 GETTER,                 \
                                 nullptr,                \
                                 Local<Value>(),         \
                                 AccessControl::DEFAULT, \
                                 PropertyAttribute::ReadOnly)

      // Properties (read-only)
      DEFINE_READONLY_PROPERTY("width", WidthGetter);
      DEFINE_READONLY_PROPERTY("height", HeightGetter);
      DEFINE_READONLY_PROPERTY("data", DataGetter);
      DEFINE_READONLY_PROPERTY("colorSpace", ColorSpaceGetter);
#undef DEFINE_READONLY_PROPERTY
#undef NAME
    }

    Local<Object> ImageData::NewInstance(Isolate *isolate, shared_ptr<::canvas::ImageData> nativeImageData)
    {
      EscapableHandleScope scope(isolate);
      return nativeImageData == nullptr
               ? scope.Escape(Local<Object>())
               : scope.Escape(ImageDataBase::NewInstance(isolate, nativeImageData).As<Object>());
    }

    Local<Object> ImageData::NewInstance(Isolate *isolate, int width, int height, const string &colorSpace)
    {
      auto nativeImageData = make_shared<::canvas::ImageData>(width, height, colorSpace);
      return NewInstance(isolate, nativeImageData);
    }

    ImageData::ImageData(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageDataBase(isolate, args)
    {
      // Constructor implementation
      if (args.Length() >= 2 && args[0]->IsNumber() && args[1]->IsNumber())
      {
        int width = args[0]->Int32Value(isolate->GetCurrentContext()).FromJust();
        int height = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();

        string colorSpace = "srgb";
        if (args.Length() >= 3 && args[2]->IsString())
        {
          String::Utf8Value colorSpaceValue(isolate, args[2]);
          colorSpace = *colorSpaceValue;
        }
        // auto nativeImageData = make_shared<::canvas::ImageData>(width, height, colorSpace);
        // SetNativeInstance(nativeImageData);
      }
      else if (args.Length() >= 3 && args[0]->IsUint8Array() && args[1]->IsNumber() && args[2]->IsNumber())
      {
        // Constructor with Uint8Array data
        Local<Uint8Array> dataArray = args[0].As<Uint8Array>();
        int width = args[1]->Int32Value(isolate->GetCurrentContext()).FromJust();
        int height = args[2]->Int32Value(isolate->GetCurrentContext()).FromJust();

        string colorSpace = "srgb";
        if (args.Length() >= 4 && args[3]->IsString())
        {
          String::Utf8Value colorSpaceValue(isolate, args[3]);
          colorSpace = *colorSpaceValue;
        }

        // Create ImageData with provided data
        // auto nativeImageData = make_shared<::canvas::ImageData>(width, height, colorSpace);
        // TODO: Copy data from Uint8Array to native ImageData
        // SetNativeInstance(nativeImageData);
      }
    }

    void ImageData::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(info.This());

      if (imageData && imageData->inner())
      {
        int width = imageData->inner()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageData::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(info.This());

      if (imageData && imageData->inner())
      {
        int height = imageData->inner()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageData::DataGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(info.This());

      if (imageData && imageData->inner())
      {
        // TODO: Return Uint8ClampedArray with image data
        // For now, return an empty Uint8ClampedArray
        size_t dataLength = imageData->inner()->width() * imageData->inner()->height() * 4;
        Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, dataLength);
        Local<Uint8ClampedArray> dataArray = Uint8ClampedArray::New(buffer, 0, dataLength);
        info.GetReturnValue().Set(dataArray);
      }
      else
      {
        Local<ArrayBuffer> buffer = ArrayBuffer::New(isolate, 0);
        Local<Uint8ClampedArray> dataArray = Uint8ClampedArray::New(buffer, 0, 0);
        info.GetReturnValue().Set(dataArray);
      }
    }

    void ImageData::ColorSpaceGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(info.This());

      string colorSpaceStr = "srgb"; // Default to "srgb"
      if (imageData && imageData->inner())
      {
        SkColorSpace *colorSpace = imageData->inner()->colorSpace();
        if (colorSpace)
        {
          if (colorSpace->isSRGB())
            colorSpaceStr = "srgb";
          else if (colorSpace->gammaIsLinear())
            colorSpaceStr = "linear";
          else
            colorSpaceStr = "custom"; // Fallback for unsupported color spaces
        }
      }
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, colorSpaceStr.c_str()).ToLocalChecked());
    }
  }
}