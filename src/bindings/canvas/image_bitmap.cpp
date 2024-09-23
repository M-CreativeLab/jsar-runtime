#include <skia/include/codec/SkCodec.h>
#include "common/utility.hpp"
#include "image_bitmap.hpp"
#include "image_data.hpp"

using namespace std;

namespace canvasbinding
{
  Napi::FunctionReference *ImageBitmap::constructor;
  void ImageBitmap::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "ImageBitmap",
                                      {
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

    if (info.Length() < 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<SharedReference<canvas::ImageBitmap>>>();
    dataImpl = external.Data()->value;

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("width", Napi::Number::New(env, dataImpl->width()));
    jsThis.Set("height", Napi::Number::New(env, dataImpl->height()));
  }

  ImageBitmap::~ImageBitmap()
  {
  }

  Napi::Value ImageBitmap::CreateImageBitmap(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      string msg = "Failed to execute 'createImageBitmap' on 'Window': 1 argument required, but only 0 present";
      Napi::TypeError::New(env, msg).ThrowAsJavaScriptException();
      return env.Undefined();
    }

    float sx = 0, sy = 0, sw = 0, sh = 0;
    if (info.Length() >= 5)
    {
      sx = info[1].ToNumber().FloatValue();
      sy = info[2].ToNumber().FloatValue();
      sw = info[3].ToNumber().FloatValue();
      sh = info[4].ToNumber().FloatValue();
    }

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    shared_ptr<canvas::ImageBitmap> impl = nullptr;
    auto imageObjectValue = info[0].ToObject();
    if (imageObjectValue.InstanceOf(ImageBitmap::constructor->Value()))
    {
      ImageBitmap *imageBitmap = ImageBitmap::Unwrap(imageObjectValue);
      impl = canvas::ImageBitmap::CreateImageBitmap(imageBitmap->dataImpl, sx, sy, sw, sh);
    }
    else if (imageObjectValue.InstanceOf(ImageData::constructor->Value()))
    {
      ImageData *imageData = ImageData::Unwrap(imageObjectValue);
      impl = canvas::ImageBitmap::CreateImageBitmap(imageData->dataImpl, sx, sy, sw, sh);
    }
    else if (imageObjectValue.InstanceOf(env.Global().Get("Blob").As<Napi::Function>()))
    {
      auto blobType = imageObjectValue.Get("type").ToString().Utf8Value();
      auto blobArrayBuffer = imageObjectValue.Get("arrayBuffer").As<Napi::Function>().Call(imageObjectValue, {});
      if (!blobArrayBuffer.IsPromise())
      {
        deferred.Reject(Napi::TypeError::New(env, "Invalid Blob instance").Value());
      }
      else
      {
        auto onResolve = [sx, sy, sw, sh, blobType](const Napi::CallbackInfo &info)
        {
          return ImageBitmap::OnBlobArrayBufferCallback(info, sx, sy, sw, sh, blobType);
        };
        auto jsPromiseObject = blobArrayBuffer.ToObject();
        return jsPromiseObject
            .Get("then")
            .As<Napi::Function>()
            .Call(jsPromiseObject, {Napi::Function::New(env, onResolve),
                                    Napi::Function::New(env, ImageBitmap::OnBlobRejectionCallback)});
      }
    }

    if (impl != nullptr)
    {
      SharedReference<canvas::ImageBitmap> reference(impl);
      auto newBitmapObject = ImageBitmap::constructor->New({Napi::External<SharedReference<canvas::ImageBitmap>>::New(env, &reference)});
      deferred.Resolve(newBitmapObject);
    }
    else
    {
      // TODO: support HTMLImageElement, SVGImageElement, HTMLVideoElement, HTMLCanvasElement, Blob and OffscreenCanvas
      // https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/createImageBitmap
      auto msg = "Failed to execute 'createImageBitmap' on 'Window': "
                 "The provided value is not of type '(HTMLImageElement or HTMLCanvasElement or ImageBitmap or OffscreenCanvas or Blob or ImageData)'";
      deferred.Reject(Napi::TypeError::New(env, msg).Value());
    }
    return deferred.Promise();
  }

  Napi::Value ImageBitmap::OnBlobArrayBufferCallback(const Napi::CallbackInfo &info, float sx, float sy, float sw, float sh, const string blobType)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() < 1 || !info[0].IsArrayBuffer())
    {
      deferred.Reject(Napi::TypeError::New(env, "Blob.arraybuffer() not resolved an ArrayBuffer.").Value());
      return deferred.Promise();
    }

    auto arrayBuffer = info[0].As<Napi::ArrayBuffer>();
    try
    {
      auto impl = canvas::ImageBitmap::CreateImageBitmap(arrayBuffer.Data(), arrayBuffer.ByteLength(), sx, sy, sw, sh);
      SharedReference<canvas::ImageBitmap> reference(impl);
      auto newBitmapObject = ImageBitmap::constructor->New({Napi::External<SharedReference<canvas::ImageBitmap>>::New(env, &reference)});
      deferred.Resolve(newBitmapObject);
    }
    catch (const runtime_error &e)
    {
      auto msg = "Failed to execute 'createImageBitmap' on 'Window': " + string(e.what());
      deferred.Reject(Napi::TypeError::New(env, msg).Value());
    }
    return deferred.Promise();
  }

  Napi::Value ImageBitmap::OnBlobRejectionCallback(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);
    deferred.Reject(info[0]);
    return deferred.Promise();
  }

  Napi::Value ImageBitmap::Close(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    dataImpl->close();
    return env.Undefined();
  }
}
