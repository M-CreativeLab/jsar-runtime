#include "./xr_hand.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace webxr_bindings
  {
    void XRHand::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->InstanceTemplate();
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Add property accessors
      InstanceReadonlyAccessor(isolate, instance, "size", &XRHand::SizeGetter);

      // Add Map-like methods
      InstanceMethod(isolate, prototype, "entries", &XRHand::Entries);
      InstanceMethod(isolate, prototype, "forEach", &XRHand::ForEach);
      InstanceMethod(isolate, prototype, "get", &XRHand::Get);
      InstanceMethod(isolate, prototype, "keys", &XRHand::Keys);
      InstanceMethod(isolate, prototype, "values", &XRHand::Values);

      // Add iterator support
      // TODO(yorkie): support Symbol?
      InstanceMethod(isolate, prototype, "Symbol.iterator", &XRHand::Iterator);
    }

    XRHand::XRHand(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : XRHandBase(isolate, args)
    {
    }

    // Property getters

    void XRHand::SizeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(Number::New(isolate, handle()->size()));
    }

    // Map-like methods

    void XRHand::Entries(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return iterator over hand joint entries
      cout << "hand.entries called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRHand::ForEach(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        return;
      }

      // TODO: Implement forEach iteration over hand joints
      cout << "hand.forEach called" << endl;
    }

    void XRHand::Get(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      if (info.Length() < 1)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Validate joint name and return corresponding XRJointSpace
      // Joint names: wrist, thumb-metacarpal, thumb-phalanx-proximal, etc.
      cout << "hand.get called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRHand::Keys(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return iterator over hand joint keys
      cout << "hand.keys called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRHand::Values(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return iterator over hand joint spaces
      cout << "hand.values called" << endl;
      info.GetReturnValue().SetNull();
    }

    void XRHand::Iterator(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      XRHand *hand = Unwrap(isolate, info.This());
      if (hand == nullptr || hand->inner() == nullptr)
      {
        info.GetReturnValue().SetNull();
        return;
      }

      // TODO: Return iterator for hand joints
      cout << "hand[Symbol.iterator] called" << endl;
      info.GetReturnValue().SetNull();
    }
  }
}