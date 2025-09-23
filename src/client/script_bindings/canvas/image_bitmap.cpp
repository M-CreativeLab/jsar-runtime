#include "image_bitmap.hpp"
#include <client/canvas/image_bitmap.hpp>

namespace script_bindings
{
  namespace canvas
  {
    using namespace v8;

    void ImageBitmap::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "ImageBitmap").ToLocalChecked());

      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      instanceTemplate->SetInternalFieldCount(1);

      // Properties (read-only)
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                    WidthGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                    HeightGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      // Methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "close").ToLocalChecked(),
                            FunctionTemplate::New(isolate, Close));
    }

    Local<Object> ImageBitmap::NewInstance(Isolate *isolate, std::shared_ptr<::canvas::ImageBitmap> nativeImageBitmap)
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance = constructor->NewInstance(context).ToLocalChecked();

      ImageBitmap *imageBitmapWrapper = ObjectWrap::Unwrap<ImageBitmap>(instance);
      imageBitmapWrapper->SetNativeInstance(nativeImageBitmap);

      return instance;
    }

    Local<Function> ImageBitmap::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    ImageBitmap::ImageBitmap(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageBitmapBase(isolate, args)
    {
      // ImageBitmap is typically created through factory methods, not constructor
      // But we can create an empty instance here
      auto nativeImageBitmap = std::make_shared<::canvas::ImageBitmap>();
      SetNativeInstance(nativeImageBitmap);
    }

    void ImageBitmap::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = ObjectWrap::Unwrap<ImageBitmap>(info.Holder());

      if (imageBitmap && imageBitmap->GetNativeInstance())
      {
        int width = imageBitmap->GetNativeInstance()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageBitmap::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = ObjectWrap::Unwrap<ImageBitmap>(info.Holder());

      if (imageBitmap && imageBitmap->GetNativeInstance())
      {
        int height = imageBitmap->GetNativeInstance()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageBitmap::Close(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = ObjectWrap::Unwrap<ImageBitmap>(info.Holder());

      if (!imageBitmap || !imageBitmap->GetNativeInstance())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid ImageBitmap instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement close method
      // This should release the image bitmap resources
      imageBitmap->GetNativeInstance()->close();
      info.GetReturnValue().SetUndefined();
    }
  }
}