#include <skia/include/codec/SkCodec.h>

#include "image_bitmap.hpp"
#include "image_data.hpp"

namespace bindings
{
  namespace canvas
  {
    Napi::FunctionReference *ImageBitmap::constructor;
    void ImageBitmap::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::Function func = DefineClass(env, "ImageBitmap",
                                        {
                                            InstanceAccessor("width", &ImageBitmap::WidthGetter, nullptr),
                                            InstanceAccessor("height", &ImageBitmap::HeightGetter, nullptr),
                                            InstanceMethod("close", &ImageBitmap::Close),
                                        });

      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      exports.Set("ImageBitmap", func);
      exports.Set("createImageBitmap", Napi::Function::New(env, ImageBitmap::CreateImageBitmap));
    }

    ImageBitmap::ImageBitmap(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ImageBitmap>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() >= 2)
      {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
      }
      if (!info[0].IsExternal())
      {
        Napi::TypeError::New(env, "Invalid arguments to create ImageBitmap instance").ThrowAsJavaScriptException();
        return;
      }

      skBitmap = static_cast<SkBitmap *>(info[0].As<Napi::External<SkBitmap>>().Data());
      width = skBitmap->width();
      height = skBitmap->height();
    }

    ImageBitmap::~ImageBitmap()
    {
      if (skBitmap != nullptr)
      {
        skBitmap->reset();
        delete skBitmap;
        skBitmap = nullptr;
      }
    }

    Napi::Value ImageBitmap::CreateImageBitmap(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 1)
      {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      auto imageObjectValue = info[0].ToObject();
      auto blobConstructorValue = env.Global().Get("Blob").As<Napi::Function>();
      Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
      Napi::External<SkBitmap> external;
      // Check `imageValue` is the correct type
      if (imageObjectValue.InstanceOf(constructor->Value()))
      {
        auto imageBitmap = ImageBitmap::Unwrap(imageObjectValue);
        auto bitmap = new SkBitmap(*imageBitmap->skBitmap);
        external = Napi::External<SkBitmap>::New(env, bitmap);
        deferred.Resolve(constructor->New({external}));
      }
      else if (imageObjectValue.InstanceOf(ImageData::constructor->Value()))
      {
        auto imageData = ImageData::Unwrap(imageObjectValue);
        auto bitmap = new SkBitmap();
        bitmap->allocN32Pixels(imageData->width, imageData->height);
        bitmap->setPixels(imageData->data.data());
        external = Napi::External<SkBitmap>::New(env, bitmap);
        deferred.Resolve(constructor->New({external}));
      }
      else if (imageObjectValue.InstanceOf(blobConstructorValue))
      {
        // auto blobTypeString = imageObjectValue.Get("type").ToString();
        auto blobArrayBuffer = imageObjectValue.Get("arrayBuffer").As<Napi::Function>().Call(imageObjectValue, {});
        if (!blobArrayBuffer.IsPromise())
        {
          deferred.Reject(Napi::TypeError::New(env, "Invalid Blob instance").Value());
        }
        else
        {
          auto jsPromiseObject = blobArrayBuffer.ToObject();
          auto then = jsPromiseObject.Get("then").As<Napi::Function>();
          return then.Call(jsPromiseObject, {Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                                                 {
              Napi::Env env = info.Env();
              Napi::HandleScope scope(env);
              Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

              auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
              auto bitmap = new SkBitmap();
              auto data = reinterpret_cast<uint8_t *>(arrayBuffer.Data());
              auto length = arrayBuffer.ByteLength();
              auto codec = SkCodec::MakeFromData(SkData::MakeWithoutCopy(data, length));
              if (codec)
              {
                SkImageInfo info = codec->getInfo().makeColorType(kN32_SkColorType);
                bitmap->allocPixels(info);
                codec->getPixels(info, bitmap->getPixels(), bitmap->rowBytes());
                deferred.Resolve(constructor->New({Napi::External<SkBitmap>::New(env, bitmap)}));
              }
              else
              {
                deferred.Reject(Napi::TypeError::New(env, "Failed to create ImageBitmap instance from Blob").Value());
              }
              return deferred.Promise(); }),
                                             Napi::Function::New(env, [](const Napi::CallbackInfo &info)
                                                                 {
              Napi::Env env = info.Env();
              Napi::HandleScope scope(env);
              Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
              deferred.Reject(info[0]);
              return deferred.Promise(); })});
        }
      }
      else
      {
        // TODO: support HTMLImageElement, SVGImageElement, HTMLVideoElement, HTMLCanvasElement, Blob and OffscreenCanvas
        // https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/createImageBitmap
        deferred.Reject(Napi::TypeError::New(env, "Unknown image type to create ImageBitmap instance").Value());
      }
      return deferred.Promise();
    }

    Napi::Value ImageBitmap::WidthGetter(const Napi::CallbackInfo &info)
    {
      return Napi::Number::New(info.Env(), width);
    }

    Napi::Value ImageBitmap::HeightGetter(const Napi::CallbackInfo &info)
    {
      return Napi::Number::New(info.Env(), height);
    }

    Napi::Value ImageBitmap::Close(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      skBitmap->reset();
      skBitmap = nullptr;
      return info.Env().Undefined();
    }
  }
}
