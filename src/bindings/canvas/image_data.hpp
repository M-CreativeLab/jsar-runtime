#pragma once

#include <napi.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>

namespace bindings
{
  namespace canvas
  {
    class ImageBitmap;
    class ImageData : public Napi::ObjectWrap<ImageData>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);
      ImageData(const Napi::CallbackInfo &info);

    private:
      Napi::Value WidthGetter(const Napi::CallbackInfo &info);
      Napi::Value HeightGetter(const Napi::CallbackInfo &info);
      Napi::Value ColorSpaceGetter(const Napi::CallbackInfo &info);
      Napi::Value DataGetter(const Napi::CallbackInfo &info);

    public:
      sk_sp<SkImage> getImage() const;
      SkBitmap getBitmap() const;

    private:
      uint32_t width;
      uint32_t height;
      SkColorType colorSpace = kRGBA_8888_SkColorType;
      std::vector<uint8_t> data;

    public:
      static Napi::FunctionReference *constructor;

    public:
      friend class ImageBitmap;
    };
  }
}
