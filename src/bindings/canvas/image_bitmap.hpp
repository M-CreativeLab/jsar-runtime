#pragma once

#include <memory>
#include <napi.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include "client/canvas/image_bitmap.hpp"
#include "./image_source.hpp"

namespace canvasbinding
{
  class ImageBitmap : public Napi::ObjectWrap<ImageBitmap>,
                      public ImageSourceWrap<canvas::ImageBitmap>
  {
  public:
    static void Init(Napi::Env env, Napi::Object exports);
    ImageBitmap(const Napi::CallbackInfo &info);
    ~ImageBitmap();

  private:
    static Napi::Value CreateImageBitmap(const Napi::CallbackInfo &info);
    static Napi::Value OnBlobArrayBufferCallback(const Napi::CallbackInfo &info,
                                                 float sx,
                                                 float sy,
                                                 float sw,
                                                 float sh,
                                                 const std::string blobType);
    static Napi::Value OnBlobRejectionCallback(const Napi::CallbackInfo &info);
    Napi::Value Close(const Napi::CallbackInfo &info);

  public:
    static thread_local Napi::FunctionReference *constructor;
  };
}
