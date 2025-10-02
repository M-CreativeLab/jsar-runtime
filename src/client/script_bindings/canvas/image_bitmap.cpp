#include "image_bitmap.hpp"
#include <client/canvas/image_bitmap.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace v8;

    void ImageBitmap::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();

      // Properties (read-only)
      InstanceReadonlyAccessor(isolate, prototypeTemplate, "width", &ImageBitmap::WidthGetter);
      InstanceReadonlyAccessor(isolate, prototypeTemplate, "height", &ImageBitmap::HeightGetter);

      // Methods
      InstanceMethod(isolate, prototypeTemplate, "close", &ImageBitmap::Close);
    }

    ImageBitmap::ImageBitmap(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageBitmapBase(isolate, args)
    {
    }

    void ImageBitmap::WidthGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = Unwrap(isolate, info.This());

      if (imageBitmap && imageBitmap->handle())
      {
        int width = imageBitmap->handle()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageBitmap::HeightGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageBitmap *imageBitmap = Unwrap(isolate, info.This());

      if (imageBitmap && imageBitmap->handle())
      {
        int height = imageBitmap->handle()->height();
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
      HandleScope scope(isolate);

      handle()->close();
      info.GetReturnValue().SetUndefined();
    }
  }
}
