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
      HandleScope scope(isolate);
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Properties (read-only)
      InstanceReadonlyAccessor(isolate, instanceTemplate, "width", &ImageData::WidthGetter);
      InstanceReadonlyAccessor(isolate, instanceTemplate, "height", &ImageData::HeightGetter);
      InstanceReadonlyAccessor(isolate, instanceTemplate, "data", &ImageData::DataGetter);
      InstanceReadonlyAccessor(isolate, instanceTemplate, "colorSpace", &ImageData::ColorSpaceGetter);
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

    void ImageData::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(isolate, info.This());

      if (imageData && imageData->handle())
      {
        int width = imageData->handle()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageData::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(isolate, info.This());

      if (imageData && imageData->handle())
      {
        int height = imageData->handle()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageData::DataGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(isolate, info.This());

      if (imageData && imageData->handle())
      {
        // TODO: Return Uint8ClampedArray with image data
        // For now, return an empty Uint8ClampedArray
        size_t dataLength = imageData->handle()->width() * imageData->handle()->height() * 4;
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

    void ImageData::ColorSpaceGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageData *imageData = Unwrap(isolate, info.This());

      string colorSpaceStr = "srgb"; // Default to "srgb"
      if (imageData && imageData->handle())
      {
        SkColorSpace *colorSpace = imageData->handle()->colorSpace();
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
