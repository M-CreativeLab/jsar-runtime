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

  Napi::Object XRInputSource::NewInstance(Napi::Env env, XRFrame *frame, XRSession *session,
                                          InputSourceInternalResetCallback resetInternal)
  {
    Napi::EscapableHandleScope scope(env);
    // auto frameExternal = Napi::External<xr::DeviceFrame>::New(env, frame);
    auto xrFrameValue = frame->Value();
    auto xrSessionValue = session->Value();
    auto resetInternalExternal = Napi::External<InputSourceInternalResetCallback>::New(env, &resetInternal);
    Napi::Object obj = constructor->New({xrFrameValue, xrSessionValue, resetInternalExternal});
    return scope.Escape(obj).ToObject();
  }

  XRInputSource::XRInputSource(const Napi::CallbackInfo &info) : Napi::ObjectWrap<XRInputSource>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 3)
    {
      Napi::TypeError::New(env, "XRInputSource constructor expects 3 argument").ThrowAsJavaScriptException();
      return;
    }
    if (!info[2].IsExternal())
    {
      Napi::TypeError::New(env, "XRInputSource constructor could not be called").ThrowAsJavaScriptException();
      return;
    }

    auto frameValue = info[0].As<Napi::Object>();
    xrFrame = Napi::Persistent(frameValue);

    auto sessionValue = info[1].As<Napi::Object>();
    xrSession = Napi::Persistent(sessionValue);

    auto resetInternalExternal = info[2].As<Napi::External<InputSourceInternalResetCallback>>();
    onResetInternal = *resetInternalExternal.Data();
    updateInternal(XRFrame::Unwrap(frameValue)); // Update internal once when the input source is created

    auto thisObject = info.This().ToObject();
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("gripSpace", GripSpaceGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("handedness", HandednessGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRayMode", TargetRayModeGetter(info), napi_enumerable));
    thisObject.DefineProperty(Napi::PropertyDescriptor::Value("targetRaySpace", TargetRaySpaceGetter(info), napi_enumerable));
  }

  XRInputSource::~XRInputSource()
  {
    delete internal;
    internal = nullptr;
    xrSession.Unref();
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

  bool XRInputSource::updateInternal(XRFrame *frame)
  {
    if (!onResetInternal || frame == nullptr)
      return false;

    auto newInternal = onResetInternal(frame->internal);
    if (newInternal == nullptr)
      return false;

    if (internal == nullptr)
      internal = new xr::TrXRInputSource(newInternal);
    else
      internal->update(newInternal);

    dispatchSelectOrSqueezeEvents();
    return true;
  }

  bool XRInputSource::dispatchSelectOrSqueezeEvents()
  {
    auto session = XRSession::Unwrap(xrSession.Value());
    if (session == nullptr)
      return false;

    if (internal->primaryActionPressed == true)
    {
      // When the primary action is pressed.
      if (primaryActionPressed == false)
      {
        // When the primary action is pressed for the first time.
        primaryActionPressed = true;
        session->onPrimaryActionStart(this, XRFrame::Unwrap(xrFrame.Value()));
      }
    }
    else
    {
      // When the primary action is not pressed.
      if (primaryActionPressed == true)
      {
        // When the primary action is released.
        primaryActionPressed = false;
        session->onPrimaryActionEnd(this, XRFrame::Unwrap(xrFrame.Value()));
      }
    }

    if (internal->squeezeActionPressed == true)
    {
      // When the squeeze action is pressed.
      if (squeezeActionPressed == false)
      {
        // When the squeeze action is pressed for the first time.
        squeezeActionPressed = true;
        session->onSqueezeActionStart(this, XRFrame::Unwrap(xrFrame.Value()));
      }
    }
    else
    {
      // When the squeeze action is not pressed.
      if (squeezeActionPressed == true)
      {
        // When the squeeze action is released.
        squeezeActionPressed = false;
        session->onSqueezeActionEnd(this, XRFrame::Unwrap(xrFrame.Value()));
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
    auto inputSourcesZone = clientContext->getXRInputSourcesZone();
    auto gazeInputSource = inputSourcesZone->getGazeInputSource();
    fprintf(stdout, "hand: %d\n", gazeInputSource->handness);
    fprintf(stdout, "targetRayMode: %d\n", gazeInputSource->targetRayMode);

    auto m = gazeInputSource->targetRayBaseMatrix;
    fprintf(stdout, "targetRayTransform: (%f %f %f %f)\n", m[0], m[1], m[2], m[3]);

    if (Length() == 0)
    {
      vector<XRInputSource *> added;
      // auto gazeInputSource = XRInputSource::Unwrap(
      //     XRInputSource::NewInstance(env, frame, session, [](xr::TrXRFrameRequest *frameRequest) -> xr::TrXRInputSource *
      //                                { return frameRequest->getGazeInputSource(); }));
      // auto leftHandInputSource = XRInputSource::Unwrap(
      //     XRInputSource::NewInstance(env, frame, session, [](xr::TrXRFrameRequest *frameRequest) -> xr::TrXRInputSource *
      //                                { return frameRequest->getHandInputSource(xr::TrHandness::Left); }));
      // auto rightHandInputSource = XRInputSource::Unwrap(
      //     XRInputSource::NewInstance(env, frame, session, [](xr::TrXRFrameRequest *frameRequest) -> xr::TrXRInputSource *
      //                                { return frameRequest->getHandInputSource(xr::TrHandness::Right); }));

      // added.push_back(gazeInputSource);
      // added.push_back(leftHandInputSource);
      // added.push_back(rightHandInputSource);
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
        inputSource->updateInternal(frame);
      }
    }
  }
}
