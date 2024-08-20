#include <array>
#include "input_source.hpp"
#include "space.hpp"
#include "session.hpp"
#include "frame.hpp"
#include "hand.hpp"
#include "rigid_transform.hpp"

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
                                          InstanceMethod("setTargetRayHitTestResult", &XRInputSource::SetTargetRayHitTestResult),
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
    clientContext = TrClientContextPerProcess::Get();

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

  Napi::Value XRInputSource::SetTargetRayHitTestResult(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() == 0)
    {
      Napi::TypeError::New(env, "Expected 1 argument at least").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    XRSession *session = XRSession::Unwrap(xrSessionRef.Value());
    if (session == nullptr)
    {
      Napi::TypeError::New(env, "Failed to get the session").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    static bool missDispatched = false;
    xr::SetInputSourceTargetRayHitTestResult req(session->id, internal->id);
    auto hit = info[0].ToBoolean().Value();
    if (!hit)
    {
      if (missDispatched)
      {
        /**
         * If the "miss" state has been dispatched, just avoid to dispatch it again for performance.
         */
        return env.Undefined();
      }
      xr::TrRayHitResult hitResult(false);
      req.setResult(false, nullptr);
      missDispatched = true;
    }
    else if (info.Length() >= 2 && info[1].IsObject())
    {
      auto jsHitTestResult = info[1].ToObject();
      if (jsHitTestResult.InstanceOf(XRRigidTransform::constructor->Value()))
      {
        auto transform = XRRigidTransform::Unwrap(jsHitTestResult);
        float *matrixValues = glm::value_ptr(transform->matrix);

        xr::TrRayHitResult hitResult(true, matrixValues);
        req.setResult(true, matrixValues);
        missDispatched = false; // When there is a new hit again, we need to reset the flag to dispatch the "miss" once again.
      }
      else
      {
        Napi::TypeError::New(env, "Expected a XRRigidTransform object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }
    else
    {
      Napi::TypeError::New(env, "Expected a XRRigidTransform object.").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    clientContext->sendXrCommand(req);
    return env.Undefined();
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
    else if (targetRayMode == xr::TrXRTargetRayMode::TransientPointer)
      return Napi::String::New(env, "transient-pointer");
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

  void checkInputSourceEnabledAndInsertTo(set<int> &targetSet, xr::TrXRInputSource *inputSource)
  {
    if (inputSource != nullptr && inputSource->enabled)
      targetSet.insert(inputSource->id);
  }

  void XRInputSourceArray::updateInputSources(XRFrame *frame,
                                              XRSession *session,
                                              InputSourcesChangedCallback onChangedCallback)
  {
    Napi::Env env = Env();
    auto inputSourcesZone = clientContext->getXRInputSourcesZone();

    /**
     * 1. Prepare sets including: added, removed, new, and old.
     */
    set<int> addedInputSourceIds;
    set<int> removedInputSourceIds;
    set<int> newInputSourceIds;
    {
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getGazeInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getMainControllerInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getTransientPointerInputSource());
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getHandInputSource(xr::TrHandness::Left));
      checkInputSourceEnabledAndInsertTo(newInputSourceIds, inputSourcesZone->getHandInputSource(xr::TrHandness::Right));
    }
    set<int> currentInputSourceIds;
    {
      // Fetch current input sources from the array.
      for (uint32_t i = 0; i < Length(); i++)
      {
        auto inputSource = XRInputSource::Unwrap(Get(i).ToObject());
        currentInputSourceIds.insert(inputSource->internal->id);
      }
    }

    /**
     * 2. Compare the new and old sets to get added and removed sets.
     */
    if (currentInputSourceIds.size() == 0)
    {
      addedInputSourceIds = newInputSourceIds;
    }
    else
    {
      set_difference(newInputSourceIds.begin(), newInputSourceIds.end(),
                     currentInputSourceIds.begin(), currentInputSourceIds.end(),
                     inserter(addedInputSourceIds, addedInputSourceIds.begin()));
      set_difference(currentInputSourceIds.begin(), currentInputSourceIds.end(),
                     newInputSourceIds.begin(), newInputSourceIds.end(),
                     inserter(removedInputSourceIds, removedInputSourceIds.begin()));
    }

    /**
     * 3. Update the new set to the current set.
     */
    currentInputSourceIds = newInputSourceIds;

    /**
     * 4. Process the removed input sources firstly.
     */
    if (removedInputSourceIds.size() > 0)
    {
      vector<XRInputSource *> removed;
      for (auto id : removedInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
          removed.push_back(inputSource);
      }
      onChangedCallback({}, removed);
    }

    /**
     * 5. Reset the array based on current input sources.
     *
     * NOTE: This is safe because we have already processed the removed input sources.
     */
    if (removedInputSourceIds.size() > 0 || addedInputSourceIds.size() > 0)
    {
      // Only update the JavaScript array object when there is a change, otherwise no need to update.
      Napi::Array tmpArray = Napi::Array::New(env);
      for (int id : currentInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
        {
          // Reuse the existed input source object.
          tmpArray.Set(tmpArray.Length(), inputSource->Value());
        }
        else
        {
          auto jsObject = XRInputSource::NewInstance(env, session, inputSourcesZone->getInputSourceById(id));
          tmpArray.Set(tmpArray.Length(), jsObject);
        }
      }
      Set("length", Napi::Number::New(env, 0));
      for (uint32_t i = 0; i < tmpArray.Length(); i++)
        Set(i, tmpArray.Get(i));
    }

    /**
     * 6. Process the added input sources.
     *
     * NOTE: We don't create any new input source objects here, just use instances in the above step.
     */
    if (addedInputSourceIds.size() > 0)
    {
      vector<XRInputSource *> added;
      for (auto id : addedInputSourceIds)
      {
        auto inputSource = getInputSourceById(id);
        if (inputSource != nullptr)
          added.push_back(inputSource);
      }
      if (added.size() > 0)
        onChangedCallback(added, {});
    }

    /**
     * 7. Dispatch `select` and `squeeze` events for all existed input sources.
     */
    for (uint32_t i = 0; i < Length(); i++)
    {
      Napi::Value value = Get(i);
      if (value.IsObject())
      {
        auto inputSource = XRInputSource::Unwrap(value.ToObject());
        inputSource->dispatchSelectOrSqueezeEvents(frame);
      }
      else
        fprintf(stderr, "Failed to dispatch events on XRInputSource(%d): value is not an object.\n", i);
    }
  }

  XRInputSource *XRInputSourceArray::getInputSourceById(int id)
  {
    for (uint32_t i = 0; i < Length(); i++)
    {
      auto inputSourceObject = Get(i).ToObject();
      auto inputSource = XRInputSource::Unwrap(inputSourceObject);
      if (inputSource->internal->id == id)
      {
        return inputSource;
      }
    }
    return nullptr;
  }
}
