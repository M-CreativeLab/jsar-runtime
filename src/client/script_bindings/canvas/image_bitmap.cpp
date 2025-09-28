#include "image_bitmap.hpp"
#include <client/canvas/image_bitmap.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace v8;

    void ImageBitmap::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Properties (read-only)
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "width").ToLocalChecked(),
                                     WidthGetter,
                                     nullptr,
                                     Local<Value>(),
                                     AccessControl::DEFAULT,
                                     PropertyAttribute::ReadOnly);
      prototypeTemplate->SetAccessor(String::NewFromUtf8(isolate, "height").ToLocalChecked(),
                                     HeightGetter,
                                     nullptr,
                                     Local<Value>(),
                                     AccessControl::DEFAULT,
                                     PropertyAttribute::ReadOnly);

      // Methods
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "close").ToLocalChecked(),
                             FunctionTemplate::New(isolate, Close));
    }

    Local<Object> ImageBitmap::NewInstance(Isolate *isolate, std::shared_ptr<::canvas::ImageBitmap> nativeBitmap)
    {
      EscapableHandleScope scope(isolate);
      return nativeBitmap == nullptr
               ? scope.Escape(Object::New(isolate))
               : scope.Escape(ImageBitmapBase::NewInstance(isolate, nativeBitmap));
    }

    ImageBitmap::ImageBitmap(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageBitmapBase(isolate, args)
    {
    }

    void ImageBitmap::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = Unwrap(isolate, info.This());

      if (imageBitmap && imageBitmap->inner())
      {
        int width = imageBitmap->inner()->width();
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
      ImageBitmap *imageBitmap = Unwrap(isolate, info.This());

      if (imageBitmap && imageBitmap->inner())
      {
        int height = imageBitmap->inner()->height();
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
      ImageBitmap *imageBitmap = Unwrap(isolate, info.This());

      if (!imageBitmap || !imageBitmap->inner())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid ImageBitmap instance").ToLocalChecked()));
        return;
      }

      imageBitmap->inner()->close();
      info.GetReturnValue().SetUndefined();
    }
  }
}
