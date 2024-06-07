#pragma once

#include <napi.h>
#include <skia/include/core/SkImage.h>
#include <skia/include/core/SkBitmap.h>
#include <skia/include/core/SkColorSpace.h>

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
      ~ImageData();

    private:
      Napi::Value WidthGetter(const Napi::CallbackInfo &info);
      Napi::Value HeightGetter(const Napi::CallbackInfo &info);
      Napi::Value ColorSpaceGetter(const Napi::CallbackInfo &info);
      Napi::Value DataGetter(const Napi::CallbackInfo &info);

    private:
      /**
       * Update the color space by name.
       * 
       * @param colorSpaceName The name of the color space: "srgb" or "display-p3".
       * @return True if the color space was updated, false otherwise.
       */
      bool updateColorSpace(std::string colorSpaceName);

    public:
      uint8_t* dataAddr();
      sk_sp<SkImage> getImage() const;
      SkBitmap getBitmap() const;

    private:
      uint32_t width;
      uint32_t height;
      sk_sp<SkColorSpace> colorSpace;
      std::string colorSpaceName;
      Napi::Reference<Napi::Uint8Array> dataArrayRef;

    public:
      static Napi::FunctionReference *constructor;

    public:
      friend class ImageBitmap;
    };
  }
}
