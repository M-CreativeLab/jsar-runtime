#include "image_source.hpp"
#include <client/canvas/image_source.hpp>

namespace script_bindings
{
  namespace canvas_bindings
  {
    using namespace v8;

    void ImageSource::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      tpl->SetClassName(String::NewFromUtf8(isolate, "ImageSource").ToLocalChecked());

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
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "naturalWidth").ToLocalChecked(),
                                    NaturalWidthGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "naturalHeight").ToLocalChecked(),
                                    NaturalHeightGetter,
                                    nullptr,
                                    Local<Value>(),
                                    AccessControl::DEFAULT,
                                    PropertyAttribute::ReadOnly);

      // Methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "getImageData").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetImageData));
    }

    ImageSource::ImageSource(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageSourceBase(isolate, args)
    {
    }

    void ImageSource::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = Unwrap(isolate, info.This());

      if (imageSource && imageSource->handle())
      {
        int width = imageSource->handle()->width();
        info.GetReturnValue().Set(Number::New(isolate, width));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageSource::HeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = Unwrap(isolate, info.This());

      if (imageSource && imageSource->handle())
      {
        int height = imageSource->handle()->height();
        info.GetReturnValue().Set(Number::New(isolate, height));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageSource::NaturalWidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = Unwrap(isolate, info.This());

      if (imageSource && imageSource->handle())
      {
        // TODO(yorkie): Consider using actual natural width if different from width
        int naturalWidth = imageSource->handle()->width();
        info.GetReturnValue().Set(Number::New(isolate, naturalWidth));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageSource::NaturalHeightGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = Unwrap(isolate, info.This());

      if (imageSource && imageSource->handle())
      {
        // TODO(yorkie): Consider using actual natural height if different from height
        int naturalHeight = imageSource->handle()->height();
        info.GetReturnValue().Set(Number::New(isolate, naturalHeight));
      }
      else
      {
        info.GetReturnValue().Set(Number::New(isolate, 0));
      }
    }

    void ImageSource::GetImageData(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = Unwrap(isolate, info.This());

      if (!imageSource || !imageSource->handle())
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid ImageSource instance").ToLocalChecked()));
        return;
      }

      // TODO: Implement getImageData method
      // This should return ImageData representing the image
      info.GetReturnValue().SetNull();
    }
  }
}
