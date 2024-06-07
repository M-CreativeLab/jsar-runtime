#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkData.h>
#include "image_data.hpp"

namespace bindings
{
  namespace canvas
  {
    Napi::FunctionReference *ImageData::constructor;

    void ImageData::Init(Napi::Env env, Napi::Object exports)
    {
      Napi::Function func = DefineClass(env, "ImageData",
                                        {
                                            InstanceAccessor("width", &ImageData::WidthGetter, nullptr),
                                            InstanceAccessor("height", &ImageData::HeightGetter, nullptr),
                                            InstanceAccessor("colorSpace", &ImageData::ColorSpaceGetter, nullptr),
                                            InstanceAccessor("data", &ImageData::DataGetter, nullptr),
                                        });

      constructor = new Napi::FunctionReference();
      *constructor = Napi::Persistent(func);
      exports.Set("ImageData", func);
    }

    ImageData::ImageData(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ImageData>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 2)
      {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
      }

      Napi::Object settingsObject;
      if (info[0].IsNumber())
      {
        if (!info[1].IsNumber())
        {
          Napi::TypeError::New(env, "ImageData(width, height) must accept 2 numbers").ThrowAsJavaScriptException();
          return;
        }
        width = info[0].As<Napi::Number>().Uint32Value();
        height = info[1].As<Napi::Number>().Uint32Value();
        if (info[2].IsObject())
          settingsObject = info[2].ToObject();

        auto dataArray = Napi::Uint8Array::New(env, width * height * 4, napi_uint8_clamped_array);
        memset(dataArray.Data(), 0, dataArray.ByteLength()); // initialize with transparent black
        dataArrayRef = Napi::Persistent(dataArray);
      }
      else if (info[0].IsTypedArray())
      {
        auto dataArrayValue = info[0].As<Napi::TypedArray>();
        if (dataArrayValue.TypedArrayType() != napi_uint8_clamped_array)
        {
          Napi::TypeError::New(env, "ImageData(data) must accept Uint8ClampedArray")
              .ThrowAsJavaScriptException();
          return;
        }
        if (!info[1].IsNumber())
        {
          Napi::TypeError::New(env, "ImageData(dataArray, width) must accept number as 2nd arg")
              .ThrowAsJavaScriptException();
          return;
        }
        char *bufferPtr = reinterpret_cast<char *>(dataArrayValue.ArrayBuffer().Data());
        auto length = dataArrayValue.ByteLength();

        // Update width and height if provided
        width = info[1].As<Napi::Number>().Uint32Value();
        if (info[2].IsNumber())
          height = info[2].As<Napi::Number>().Uint32Value();
        else
          height = length / 4 / width;

        // Update data with the provided buffer
        auto dataArray = Napi::Uint8Array::New(env,
                                               dataArrayValue.ElementLength(),
                                               dataArrayValue.ArrayBuffer(),
                                               dataArrayValue.ByteOffset(),
                                               napi_uint8_clamped_array);
        dataArrayRef = Napi::Persistent(dataArray);

        if (info.Length() >= 4 && info[3].IsObject())
          settingsObject = info[3].ToObject();
      }

      if (!settingsObject.IsEmpty() && settingsObject.Has("colorSpace"))
      {
        auto colorSpaceValue = settingsObject.Get("colorSpace");
        if (colorSpaceValue.IsString())
        {
          if (!updateColorSpace(colorSpaceValue.ToString().Utf8Value()))
          {
            Napi::TypeError::New(env, "Invalid colorSpace value, only supported \"srgb\" and \"display-p3\"")
                .ThrowAsJavaScriptException();
            return;
          }
        }
      }
    }

    ImageData::~ImageData()
    {
      dataArrayRef.Unref();
    }

    Napi::Value ImageData::WidthGetter(const Napi::CallbackInfo &info)
    {
      return Napi::Number::New(info.Env(), width);
    }

    Napi::Value ImageData::HeightGetter(const Napi::CallbackInfo &info)
    {
      return Napi::Number::New(info.Env(), height);
    }

    Napi::Value ImageData::ColorSpaceGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return Napi::String::New(env, colorSpaceName);
    }

    Napi::Value ImageData::DataGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      return dataArrayRef.Value();
    }

    bool ImageData::updateColorSpace(std::string colorSpaceName)
    {
      if (colorSpaceName == "srgb" || colorSpaceName == "display-p3")
      {
        this->colorSpaceName = colorSpaceName;
        if (colorSpaceName == "srgb")
          colorSpace = SkColorSpace::MakeSRGB();
        else if (colorSpaceName == "display-p3")
          colorSpace = SkColorSpace::MakeRGB(SkNamedTransferFn::k2Dot2, SkNamedGamut::kDisplayP3);
        return true;
      }
      else
      {
        return false;
      }
    }

    uint8_t* ImageData::dataAddr()
    {
      auto dataArray = dataArrayRef.Value().As<Napi::Uint8Array>();
      return dataArray.Data() + dataArray.ByteOffset();
    }

    sk_sp<SkImage> ImageData::getImage() const
    {
      SkImageInfo imageInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
      auto dataArray = dataArrayRef.Value().As<Napi::Uint8Array>();
      auto pixels = SkData::MakeWithoutCopy(dataArray.Data() + dataArray.ByteOffset(), dataArray.ByteLength());
      auto image = SkImages::RasterFromData(imageInfo, pixels, imageInfo.minRowBytes());
      return image;
    }

    SkBitmap ImageData::getBitmap() const
    {
      auto dataArray = dataArrayRef.Value().As<Napi::Uint8Array>();
      void *pixelsData = dataArray.Data() + dataArray.ByteOffset();
      SkImageInfo imageInfo = SkImageInfo::Make(width, height, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
      SkBitmap bitmap;
      bitmap.installPixels(imageInfo, pixelsData, imageInfo.minRowBytes());
      return bitmap;
    }
  } // namespace canvas
}
