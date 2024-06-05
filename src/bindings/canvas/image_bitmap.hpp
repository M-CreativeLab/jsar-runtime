#pragma once

#include <napi.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>

namespace bindings
{
  namespace canvas
  {
    class ImageBitmap : public Napi::ObjectWrap<ImageBitmap>
    {
    public:
      static void Init(Napi::Env env, Napi::Object exports);
      ImageBitmap(const Napi::CallbackInfo &info);
      ~ImageBitmap();

    public:
      SkBitmap *getSkBitmap() { return skBitmap; }

    private:
      static Napi::Value CreateImageBitmap(const Napi::CallbackInfo &info);
      Napi::Value Close(const Napi::CallbackInfo &info);

    private:
      uint32_t width;
      uint32_t height;
      SkBitmap *skBitmap;

    public:
      static Napi::FunctionReference *constructor;
    };
  }
}
