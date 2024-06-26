#include "input_source.hpp"
#include "space.hpp"
#include "session.hpp"
#include "frame.hpp"
#include "hand.hpp"

namespace bindings
{
  Napi::FunctionReference *XRInputSource::constructor;
  Napi::FunctionReference *XRInputSourceArray::constructor;

  Napi::Object XRInputSource::Init(Napi::Env env, Napi::Object exports)
  {
    Napi::Function func = DefineClass(env, "XRInputSource",
                                      {
                                          InstanceAccessor("hand", &XRInputSource::HandGetter, nullptr),
                                          InstanceAccessor("gamepad", &XRInputSource::GamepadGetter, nullptr),
                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    exports.Set("XRInputSource", func);
    return exports;
  }

  Napi::Object XRInputSource::NewInstance(Napi::Env env, XRSession *session, xr::TrXRInputSource *internal)
  {
    Napi::EscapableHandleScope scope(env);
    auto xrSessionValue = session->Value();
    auto internalExternal = Napi::External<xr::TrXRInputSource>::New(env, internal);
    Napi::Object obj = constructor->New({xrSessionValue, internalExternal});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2)
    {
      Napi::TypeError::New(env, "XRInputSource constructor expects 2 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[1].IsExternal())
    {
      Napi::TypeError::New(env, "XRInputSource constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto sessionValue = info[0].As<Napi::Object>();
    xrSessionRef = Napi::Persistent(sessionValue);

    auto internalExternal = info[1].As<Napi::External<xr::TrXRInputSource>>();
    internal = internalExternal.Data();

    auto thisObject = info.This().ToObject();
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gripSpace", GripSpaceGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("handedness", HandednessGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRayMode", TargetRayModeGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRaySpace", TargetRaySpaceGetter(info), napi_enumerable));
  }

  XRInputSource::~XRInputSource()
  {
    internal = nullptr;
    xrSessionRef.Unref();
  }

  Napi::Value XRInputSource::GamepadGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value XRInputSource::GripSpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, true);
  }

  Napi::Value XRInputSource::HandGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return XRHand::NewInstance(env, internal);
  }

  Napi::Value XRInputSource::HandednessGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto handness = internal->handness;
    if (handness == xr::TrHandness::Left)
      return Napi::String::New(env, "left");
    else if (handness == xr::TrHandness::Right)
      return Napi::String::New(env, "right");
    else
      return Napi::String::New(env, "none");
  }

  Napi::Value XRInputSource::TargetRayModeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto targetRayMode = internal->targetRayMode;
    if (targetRayMode == xr::TrXRTargetRayMode::Gaze)
      return Napi::String::New(env, "gaze");
    else if (targetRayMode == xr::TrXRTargetRayMode::TrackedPointer)
      return Napi::String::New(env, "tracked-pointer");
    else if (targetRayMode == xr::TrXRTargetRayMode::Screen)
      return Napi::String::New(env, "screen");
    else
      return Napi::String::New(env, "tracked-pointer");
  }

  Napi::Value XRInputSource::TargetRaySpaceGetter(const Napi::CallbackInfo &info)
  {
    return XRTargetRayOrGripSpace::NewInstance(info.Env(), internal, false);
  }

  bool XRInputSource::dispatchSelectOrSqueezeEvents(XRFrame *frame)
  {
    auto session = XRSession::Unwrap(xrSessionRef.Value());
    if (session == nullptr)
      return false;

    if (internal->primaryActionPressed == true)
    {
      // When the primary action is pressed.
      if (primaryActionPressed == false)
      {
        // When the primary action is pressed for the first time.
        primaryActionPressed = true;
        session->onPrimaryActionStart(this, frame);
      }
    }
    else
    {
      // When the primary action is not pressed.
      if (primaryActionPressed == true)
      {
        // When the primary action is released.
        primaryActionPressed = false;
        session->onPrimaryActionEnd(this, frame);
      }
    }

    if (internal->squeezeActionPressed == true)
    {
      // When the squeeze action is pressed.
      if (squeezeActionPressed == false)
      {
        // When the squeeze action is pressed for the first time.
        squeezeActionPressed = true;
        session->onSqueezeActionStart(this, frame);
      }
    }
    else
    {
      // When the squeeze action is not pressed.
      if (squeezeActionPressed == true)
      {
        // When the squeeze action is released.
        squeezeActionPressed = false;
        session->onSqueezeActionEnd(this, frame);
      }
    }
    return true;
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
    clientContext = TrClientContextPerProcess::Get();
    assert(clientContext != nullptr);
  }

  XRInputSourceArray::~XRInputSourceArray()
  {
  }

  void XRInputSourceArray::updateInputSources(XRFrame *frame,
                                              XRSession *session,
                                              InputSourcesChangedCallback onChangedCallback)
  {
    Napi::Env env = Env();
    if (Length() == 0)
    {
      auto inputSourcesZone = clientContext->getXRInputSourcesZone();
      vector<XRInputSource *> added;
      auto gazeInputSourceValue = XRInputSource::NewInstance(env, session, inputSourcesZone->getGazeInputSource());
      auto leftHandInputSourceValue = XRInputSource::NewInstance(env, session, inputSourcesZone->getHandInputSource(xr::TrHandness::Left));
      auto rightHandInputSourceValue = XRInputSource::NewInstance(env, session, inputSourcesZone->getHandInputSource(xr::TrHandness::Right));

      added.push_back(XRInputSource::Unwrap(gazeInputSourceValue));
      added.push_back(XRInputSource::Unwrap(leftHandInputSourceValue));
      added.push_back(XRInputSource::Unwrap(rightHandInputSourceValue));
      // TODO: Add gamepad input sources
      onChangedCallback(added, {});

      for (uint32_t i = 0; i < added.size(); i++)
        Set(i, added[i]->Value());
      Set("length", Napi::Value::From(env, added.size()));
    }
    else
    {
      // When the length is not zero, we need to update the internal input sources
      for (uint32_t i = 0; i < Length(); i++)
      {
        auto inputSource = XRInputSource::Unwrap(Get(i).ToObject());
        inputSource->dispatchSelectOrSqueezeEvents(frame);
      }
    }
  }
}
