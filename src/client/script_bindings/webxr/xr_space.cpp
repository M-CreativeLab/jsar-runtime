#include <iostream>
#include "./xr_space.hpp"
#include "./xr_rigid_transform.hpp"

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

    // Methods

    void XRReferenceSpace::GetOffsetReferenceSpace(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (info.Length() < 1)
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "getOffsetReferenceSpace requires 1 argument").ToLocalChecked()));
        return;
      }

      // Validate the argument type
      if (!XRRigidTransform::IsInstanceOf(isolate, info[0]))
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Argument must be an XRRigidTransform object").ToLocalChecked()));
        return;
      }

      // Extract the offset transform from the argument
      auto offsetTransform = XRRigidTransform::Unwrap(isolate, info[0].As<Object>());
      if (offsetTransform == nullptr || !offsetTransform->hasData())
      {
        isolate->ThrowException(Exception::TypeError(
          String::NewFromUtf8(isolate, "Invalid XRRigidTransform object").ToLocalChecked()));
        return;
      }

      auto offsetSpace = handle()->getOffsetReferenceSpace(offsetTransform->handle()->matrix());
      info.GetReturnValue().Set(
        XRReferenceSpace::NewInstance(isolate,
                                      make_shared<client_xr::XRReferenceSpace>(offsetSpace)));
    }
  }
}
