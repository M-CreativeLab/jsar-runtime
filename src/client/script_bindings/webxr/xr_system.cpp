#include "./xr_system.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // static
    void XRSystem::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "isSessionSupported").ToLocalChecked(),
                            FunctionTemplate::New(isolate, IsSessionSupported));

      instanceTemplate->Set(String::NewFromUtf8(isolate, "requestSession").ToLocalChecked(),
                            FunctionTemplate::New(isolate, RequestSession));
    }

    // static
    Local<Object> XRSystem::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRSystem> nativeSystem)
    {
      EscapableHandleScope scope(isolate);

      if (nativeSystem == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>::NewInstance(isolate, nativeSystem).As<Object>());
    }

    // static
    Local<Function> XRSystem::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>::Initialize(isolate);
    }

    XRSystem::XRSystem(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>(isolate, args)
    {
    }

    // Methods

    // static
    void XRSystem::IsSessionSupported(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSystem *system = scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>::Unwrap(info.This());
      if (system == nullptr || system->inner() == nullptr)
      {
        // Return a rejected promise
        Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
        resolver->Reject(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "XRSystem not available").ToLocalChecked()).Check();
        info.GetReturnValue().Set(resolver->GetPromise());
        return;
      }

      // TODO: Check actual session support from native system implementation
      // Should query native XR runtime for the requested session mode support
      cout << "system.isSessionSupported called" << endl;

      // Return a resolved promise for now
      Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
      resolver->Resolve(isolate->GetCurrentContext(), Boolean::New(isolate, true)).Check();
      info.GetReturnValue().Set(resolver->GetPromise());
    }

    // static
    void XRSystem::RequestSession(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRSystem *system = scripting_base::ObjectWrap<XRSystem, client_xr::XRSystem>::Unwrap(info.This());
      if (system == nullptr || system->GetNativeInstance() == nullptr)
      {
        // Return a rejected promise
        Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
        resolver->Reject(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "XRSystem not available").ToLocalChecked()).Check();
        info.GetReturnValue().Set(resolver->GetPromise());
        return;
      }

      // TODO: Validate session mode and options, create actual XRSession from native system
      // Should handle 'inline', 'immersive-vr', 'immersive-ar' modes
      cout << "system.requestSession called" << endl;

      // Return a rejected promise for now (no session implementation yet)
      Local<Promise::Resolver> resolver = Promise::Resolver::New(isolate->GetCurrentContext()).ToLocalChecked();
      resolver->Reject(isolate->GetCurrentContext(), String::NewFromUtf8(isolate, "Session creation not implemented").ToLocalChecked()).Check();
      info.GetReturnValue().Set(resolver->GetPromise());
    }
  }
}