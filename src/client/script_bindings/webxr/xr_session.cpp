#include "./xr_session.hpp"
#include "./xr_space.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRSession::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "inputSources", &XRSession::InputSourcesGetter);
      InstanceReadonlyAccessor(isolate, instance, "renderState", &XRSession::RenderStateGetter);
      InstanceReadonlyAccessor(isolate, instance, "environmentBlendMode", &XRSession::EnvironmentBlendModeGetter);
      InstanceReadonlyAccessor(isolate, instance, "enabledFeatures", &XRSession::EnabledFeaturesGetter);

      // Add methods
      InstanceMethod(isolate, prototype, "requestAnimationFrame", &XRSession::RequestAnimationFrame);
      InstanceMethod(isolate, prototype, "cancelAnimationFrame", &XRSession::CancelAnimationFrame);
      InstanceMethod(isolate, prototype, "updateRenderState", &XRSession::UpdateRenderState);
      InstanceMethod(isolate, prototype, "requestReferenceSpace", &XRSession::RequestReferenceSpace);
      InstanceMethod(isolate, prototype, "end", &XRSession::End);
    }

    Local<Object> XRSession::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRSession> nativeSession)
    {
      EscapableHandleScope scope(isolate);

      if (nativeSession == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(XRSessionBase::NewInstance(isolate, nativeSession).As<Object>());
      }
    }

    XRSession::XRSession(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRSessionBase(isolate, args, true)
    {
    }

    // Property getters

    void XRSession::InputSourcesGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement inputSources getter - return XRInputSourceArray
      cout << "inputSources getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRSession::RenderStateGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement renderState getter - return XRRenderState
      cout << "renderState getter called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRSession::EnvironmentBlendModeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().SetEmptyString();
        return;
      }

      // TODO: Get actual environment blend mode from session
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, "opaque").ToLocalChecked());
    }

    void XRSession::EnabledFeaturesGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().Set(Array::New(isolate, 0));
        return;
      }

      // TODO: Implement enabledFeatures getter - return array of enabled features
      cout << "enabledFeatures getter called" << endl;
      info.GetReturnValue().Set(Array::New(isolate, 0));
    }

    // Methods

    void XRSession::RequestAnimationFrame(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1 || !info[0]->IsFunction())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "requestAnimationFrame requires a callback function").ToLocalChecked()));
        return;
      }

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().Set(Integer::New(isolate, 0));
        return;
      }

      // TODO: Implement requestAnimationFrame with proper frame callback handling
      cout << "requestAnimationFrame called" << endl;
      info.GetReturnValue().Set(Integer::New(isolate, 1)); // Return dummy handle
    }

    void XRSession::CancelAnimationFrame(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        return;
      }

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement cancelAnimationFrame
      cout << "cancelAnimationFrame called" << endl;
    }

    void XRSession::UpdateRenderState(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement updateRenderState with proper render state handling
      cout << "updateRenderState called" << endl;
    }

    void XRSession::RequestReferenceSpace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "requestReferenceSpace requires 1 argument").ToLocalChecked()));
        return;
      }

      XRSession *session = Unwrap(isolate, info.This());
      if (session == nullptr || session->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      String::Utf8Value referenceSpaceType(isolate, info[0]);

      // TODO: Implement requestReferenceSpace with proper space creation
      cout << "requestReferenceSpace called with type: " << *referenceSpaceType << endl;

      // Return a Promise for now (in full implementation, this should return a proper Promise)
      info.GetReturnValue().SetNull();
    }

    void XRSession::End(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      // No arguments expected
      handle()->end();
      info.GetReturnValue().SetUndefined();
    }
  }
}
