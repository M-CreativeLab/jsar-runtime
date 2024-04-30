#include "input_source.hpp"
#include "space.hpp"

namespace bindings
{
  Napi::FunctionReference *XRInputSource::constructor;
  Napi::FunctionReference *XRInputSourceArray::constructor;

  Napi::Object XRInputSource::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSource", {});
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSource", func);
    return exports;
  }

  Napi::Object XRInputSource::NewInstance(Napi::Env env, xr::InputSource *inputSource)
  {
    Napi::EscapableHandleScope scope(env);
    auto instance = Napi::External<xr::InputSource>::New(env, inputSource);
    Napi::Object obj = constructor->New({instance});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1)
    {
      Napi::TypeError::New(env, "XRInputSource constructor expects 1 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[0].IsExternal())
    {
      Napi::TypeError::New(env, "XRInputSource constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<xr::InputSource>>();
    internal = external.Data();

    auto thisObject = info.This().ToObject();
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gamepad", GamepadGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gripSpace", GripSpaceGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("hand", HandGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("handness", HandnessGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRayMode", TargetRayModeGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRaySpace", TargetRaySpaceGetter(info), napi_enumerable));
  }

  XRInputSource::~XRInputSource()
  {
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, true);
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Value();
  }

  Napi::Value XRInputSource::HandnessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto handness = internal->handness;
    if (handness == xr::Handness::Left)
      return Napi::String::New(env, "left");
    else if (handness == xr::Handness::Right)
      return Napi::String::New(env, "right");
    else
      return Napi::String::New(env, "none");
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto targetRayMode = internal->targetRayMode;
    if (targetRayMode == xr::TargetRayMode::Gaze)
      return Napi::String::New(env, "gaze");
    else if (targetRayMode == xr::TargetRayMode::TrackedPointer)
      return Napi::String::New(env, "tracked-pointer");
    else if (targetRayMode == xr::TargetRayMode::Screen)
      return Napi::String::New(env, "screen");
    else
      return Napi::String::New(env, "tracked-pointer");
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, false);
  }

  XRInputSourceArray XRInputSourceArray::New(Napi::Env env)
  {
    napi_value value;
    napi_status status = napi_create_array(env, &value);
    NAPI_THROW_IF_FAILED(env, status, XRInputSourceArray());
    return XRInputSourceArray(env, value);
  }

  XRInputSourceArray::XRInputSourceArray(napi_env env, napi_value value) : Napi::Array(env, value)
  {
    device = xr::Device::GetInstance();
    if (device == nullptr)
    {
      Napi::Error::New(env, "XRInputSourceArray: Device is not initialized").ThrowAsJavaScriptException();
      return;
    }
  }

  XRInputSourceArray::~XRInputSourceArray()
  {
  }

  void XRInputSourceArray::updateInputSources(InputSourcesChangedCallback onChangedCallback)
  {
    if (device == nullptr || !device->enabled())
      return;

    Napi::Env env = Env();
    if (Length() == 0)
    {
      vector<XRInputSource *> added;
      auto gazeInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, device->getGazeInputSource()));
      auto leftHandInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, device->getHandInputSource(xr::Handness::Left)));
      auto rightHandInputSource = XRInputSource::Unwrap(
          XRInputSource::NewInstance(env, device->getHandInputSource(xr::Handness::Right)));

      added.push_back(gazeInputSource);
      added.push_back(leftHandInputSource);
      added.push_back(rightHandInputSource);
      onChangedCallback(added, {});

      for (uint32_t i = 0; i < added.size(); i++)
        Set(i, added[i]->Value());
      Set("length", Napi::Value::From(env, added.size()));
    }
    else
    {
      // TODO
    }
  }
}
