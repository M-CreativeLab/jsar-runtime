#include "./xr_space.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    // XRSpace implementation

    void XRSpace::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);

      // Set up the instance template
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // XRSpace is mostly used as a base class and marker interface
      // Most functionality is in derived classes
    }

    XRSpace::XRSpace(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRSpaceBase(isolate, args, true)
    {
    }

    // XRReferenceSpace implementation

    void XRReferenceSpace::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add methods
      InstanceMethod(isolate,
                     prototype,
                     "getOffsetReferenceSpace",
                     &XRReferenceSpace::GetOffsetReferenceSpace);
    }

    XRReferenceSpace::XRReferenceSpace(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRReferenceSpaceBase(isolate, args, true)
    {
    }

    // Methods

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
