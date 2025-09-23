#include "image_source.hpp"
#include <client/canvas/image_source.hpp>

namespace script_bindings
{
  namespace canvas
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

    Local<Object> ImageSource::NewInstance(Isolate *isolate, std::shared_ptr<::canvas::ImageSource> nativeImageSource)
    {
      Local<Function> constructor = GetConstructorFunction(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Local<Object> instance = constructor->NewInstance(context).ToLocalChecked();

      ImageSource *imageSourceWrapper = ObjectWrap::Unwrap<ImageSource>(instance);
      imageSourceWrapper->SetNativeInstance(nativeImageSource);

      return instance;
    }

    Local<Function> ImageSource::Initialize(Isolate *isolate)
    {
      return ObjectWrap::Initialize(isolate);
    }

    ImageSource::ImageSource(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : ImageSourceBase(isolate, args)
    {
      // Constructor implementation
      auto nativeImageSource = std::make_shared<::canvas::ImageSource>();
      SetNativeInstance(nativeImageSource);
    }

    void ImageSource::WidthGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      ImageSource *imageSource = ObjectWrap::Unwrap<ImageSource>(info.Holder());

      if (imageSource && imageSource->GetNativeInstance())
      {
        int width = imageSource->GetNativeInstance()->width();
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
      ImageSource *imageSource = ObjectWrap::Unwrap<ImageSource>(info.Holder());

      if (imageSource && imageSource->GetNativeInstance())
      {
        int height = imageSource->GetNativeInstance()->height();
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
      ImageSource *imageSource = ObjectWrap::Unwrap<ImageSource>(info.Holder());

      if (imageSource && imageSource->GetNativeInstance())
      {
        int naturalWidth = imageSource->GetNativeInstance()->naturalWidth();
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
      ImageSource *imageSource = ObjectWrap::Unwrap<ImageSource>(info.Holder());

      if (imageSource && imageSource->GetNativeInstance())
      {
        int naturalHeight = imageSource->GetNativeInstance()->naturalHeight();
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
      ImageSource *imageSource = ObjectWrap::Unwrap<ImageSource>(info.Holder());

      if (!imageSource || !imageSource->GetNativeInstance())
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