#include <string>
#include <vector>
#include <skia/include/core/SkImageInfo.h>
#include <skia/include/core/SkPixmap.h>
#include <skia/include/core/SkData.h>
#include "common/utility.hpp"
#include "image_data.hpp"

using namespace std;

namespace canvasbinding
{
  Napi::FunctionReference *ImageData::constructor;

  void ImageData::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "ImageData",
                                      {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("ImageData", func);
  }

  Napi::Object ImageData::NewInstance(Napi::Env env, std::shared_ptr<canvas::ImageData> dataImpl)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<canvas::ImageData> reference(dataImpl);
    auto obj = constructor->New({Napi::External<SharedReference<canvas::ImageData>>::New(env, &reference)});
    return scope.Escape(obj).ToObject();
  }

  Napi::Object ImageData::NewInstance(Napi::Env env, float width, float height, std::string colorSpaceName)
  {
    Napi::EscapableHandleScope scope(env);
    auto obj = constructor->New({Napi::Number::New(env, width),
                                 Napi::Number::New(env, height),
                                 Napi::String::New(env, colorSpaceName)});
    return scope.Escape(obj).ToObject();
  }

  ImageData::ImageData(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<ImageData>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return;
    }

    bool skipDataImplConstructing = false;
    size_t width, height;
    vector<char> dataArray;
    string colorSpaceName = "srgb";
    Napi::Object settingsObject;
    if (info[0].IsExternal())
    {
      auto external = info[0].As<Napi::External<SharedReference<canvas::ImageData>>>();
      dataImpl = external.Data()->value;
      skipDataImplConstructing = true;
    }
    else if (info[0].IsNumber())
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

      auto dataArrayBuffer = dataArrayValue.ArrayBuffer();
      char *dataBufferPtr = reinterpret_cast<char *>(dataArrayBuffer.Data());
      dataArray.resize(dataArrayBuffer.ByteLength());
      dataArray.assign(dataBufferPtr, dataBufferPtr + dataArrayBuffer.ByteLength());

      // Update width and height if provided
      width = info[1].As<Napi::Number>().Uint32Value();
      if (info[2].IsNumber())
        height = info[2].As<Napi::Number>().Uint32Value();
      else
        height = dataArrayValue.ByteLength() / 4 / width;

      if (info.Length() >= 4 && info[3].IsObject())
        settingsObject = info[3].ToObject();
    }

    if (!settingsObject.IsEmpty() && settingsObject.Has("colorSpace"))
    {
      auto colorSpaceValue = settingsObject.Get("colorSpace");
      if (colorSpaceValue.IsString())
        colorSpaceName = colorSpaceValue.As<Napi::String>().Utf8Value();
    }

    if (!skipDataImplConstructing)
    {
      if (dataArray.empty())
        dataImpl = make_shared<canvas::ImageData>(width, height, colorSpaceName);
      else
        dataImpl = make_shared<canvas::ImageData>(dataArray, width, height, colorSpaceName);
    }

    auto jsThis = info.This().ToObject();
    Napi::ArrayBuffer arrayBuffer = Napi::ArrayBuffer::New(env, dataImpl->addr(), dataImpl->computeByteSize());
    jsThis.Set("data", Napi::Uint8Array::New(env, arrayBuffer.ByteLength(), arrayBuffer, 0, napi_uint8_clamped_array));
    jsThis.Set("width", Napi::Number::New(env, dataImpl->width()));
    jsThis.Set("height", Napi::Number::New(env, dataImpl->height()));
    jsThis.Set("colorSpace", Napi::String::New(env, dataImpl->colorSpaceName()));
  }

  ImageData::~ImageData()
  {
  }
}
