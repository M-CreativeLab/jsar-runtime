#include <skia/include/core/SkImageInfo.h>
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

      if (info.Length() >= 2)
      {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
      }

      if (info[0].IsNumber())
      {
        if (!info[1].IsNumber())
        {
          Napi::TypeError::New(env, "ImageData(width, height) must accept 2 numbers").ThrowAsJavaScriptException();
          return;
        }
        width = info[0].As<Napi::Number>().Uint32Value();
        height = info[1].As<Napi::Number>().Uint32Value();
        if (info[2].IsObject() && info[2].ToObject().Has("colorSpace"))
        {
          auto colorSpaceValue = info[2].ToObject().Get("colorSpace");
          if (colorSpaceValue.IsString())
          {
            std::string colorSpaceString = colorSpaceValue.As<Napi::String>().Utf8Value();
            if (colorSpaceString == "srgb")
            {
              colorSpace = SkColorType::kRGB_888x_SkColorType;
            }
            else if (colorSpaceString == "display-p3")
            {
              colorSpace = SkColorType::kRGBA_F16_SkColorType;
            }
            else
            {
              Napi::TypeError::New(env, "Invalid colorSpace value, only supported \"srgb\" and \"display-p3\"")
                  .ThrowAsJavaScriptException();
              return;
            }
          }
        }
        data.resize(width * height * 4);
        data.assign(data.size(), 0);
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
        auto offset = dataArrayValue.ByteOffset();
        auto length = dataArrayValue.ByteLength();
        data.resize(length);
        std::copy(bufferPtr + offset, bufferPtr + offset + length, data.begin());

        width = info[1].As<Napi::Number>().Uint32Value();
        height = length / 4 / width;
        // TODO: support colorSpace display-p3
      }
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

      if (colorSpace == SkColorType::kRGB_888x_SkColorType)
        return Napi::String::New(env, "srgb");
      else if (colorSpace == SkColorType::kRGBA_F16_SkColorType)
        return Napi::String::New(env, "display-p3");
      else
        return Napi::String::New(env, "unknown");
    }

    Napi::Value ImageData::DataGetter(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);
      
      auto dataArray = Napi::Uint8Array::New(env, data.size());
      std::copy(data.begin(), data.end(), dataArray.Data());
      return dataArray;
    }
  } // namespace canvas
}
