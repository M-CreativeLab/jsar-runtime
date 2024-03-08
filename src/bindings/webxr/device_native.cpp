#include "device_native.hpp"
#include "xr/device.hpp"

namespace bindings
{
  Napi::FunctionReference *XRDeviceNative::constructor;

  Napi::Object XRDeviceNative::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function tpl = DefineClass(env, "XRDeviceNative",
                                     {InstanceMethod("isSessionSupported", &XRDeviceNative::IsSessionSupported),
                                      InstanceMethod("requestSession", &XRDeviceNative::RequestSession),
                                      InstanceMethod("requestFrameOfReferenceTransform", &XRDeviceNative::RequestFrameOfReferenceTransform)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(tpl);
    env.SetInstanceData(constructor);

    exports.Set("XRDeviceNative", tpl);
    return exports;
  }

  XRDeviceNative::XRDeviceNative(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRDeviceNative>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
  }

  Napi::Value XRDeviceNative::IsSessionSupported(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value XRDeviceNative::RequestSession(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: expected a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = info[0].As<Napi::Number>().Int32Value();
    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestSession: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    return Napi::Boolean::New(env, device->requestSession(id));
  }

  Napi::Value XRDeviceNative::RequestFrameOfReferenceTransform(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 2)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: expected 2 arguments")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[0].IsNumber())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: sessionId must be a number")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }
    if (!info[1].IsString())
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: type must be a string")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto id = info[0].As<Napi::Number>().Int32Value();
    auto typeStr = info[1].As<Napi::String>().Utf8Value();
    if (typeStr != "local" && typeStr != "viewer")
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: expected 'local' or 'viewer'")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    auto device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::TypeError::New(env, "XRDeviceNative::RequestFrameOfReferenceTransform: device is not initialized")
          .ThrowAsJavaScriptException();
      return env.Undefined();
    }

    if (typeStr == "local")
    {
      auto transform = device->getLocalTransform(id);
      if (transform != nullptr)
      {
        auto array = Napi::Float32Array::New(env, 16);
        for (int i = 0; i < 16; i++)
          array[i] = Napi::Number::New(env, transform[i]);
        return array;
      }
    }
    else if (typeStr == "viewer")
    {
      auto transform = device->getViewerTransform();
      if (transform != nullptr)
      {
        auto array = Napi::Float32Array::New(env, 16);
        for (int i = 0; i < 16; i++)
          array[i] = Napi::Number::New(env, transform[i]);
        return array;
      }
    }
    return env.Null();
  }
}
