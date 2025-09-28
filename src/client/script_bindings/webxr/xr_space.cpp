#include "./xr_space.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // XRSpace implementation

    // static
    void XRSpace::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // XRSpace is mostly used as a base class and marker interface
      // Most functionality is in derived classes
    }

    // static
    Local<Object> XRSpace::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRSpace> nativeSpace)
    {
      EscapableHandleScope scope(isolate);

      if (nativeSpace == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(XRSpaceBase::NewInstance(isolate, nativeSpace).As<Object>());
    }

    // static
    Local<Function> XRSpace::Initialize(Isolate *isolate)
    {
      return XRSpaceBase::Initialize(isolate);
    }

    XRSpace::XRSpace(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRSpaceBase(isolate, args)
    {
    }

    // XRReferenceSpace implementation

    // static
    void XRReferenceSpace::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Add methods
      instanceTemplate->Set(String::NewFromUtf8(isolate, "getOffsetReferenceSpace").ToLocalChecked(),
                            FunctionTemplate::New(isolate, GetOffsetReferenceSpace));
    }

    // static
    Local<Object> XRReferenceSpace::NewInstance(Isolate *isolate, std::shared_ptr<client_xr::XRReferenceSpace> nativeSpace)
    {
      EscapableHandleScope scope(isolate);

      if (nativeSpace == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<XRReferenceSpace, client_xr::XRReferenceSpace, XRSpace>::NewInstance(isolate, nativeSpace).As<Object>());
    }

    // static
    Local<Function> XRReferenceSpace::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<XRReferenceSpace, client_xr::XRReferenceSpace, XRSpace>::Initialize(isolate);
    }

    XRReferenceSpace::XRReferenceSpace(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<XRReferenceSpace, client_xr::XRReferenceSpace, XRSpace>(isolate, args)
    {
    }

    // Methods

    // static
    void XRReferenceSpace::GetOffsetReferenceSpace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "getOffsetReferenceSpace requires 1 argument").ToLocalChecked()));
        return;
      }

      XRReferenceSpace *referenceSpace = Unwrap(isolate, info.This());
      if (referenceSpace == nullptr || referenceSpace->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Implement getOffsetReferenceSpace with proper XRRigidTransform handling
      cout << "getOffsetReferenceSpace called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}
