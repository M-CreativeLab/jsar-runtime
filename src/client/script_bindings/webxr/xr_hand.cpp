#include <iostream>
#include <client/xr/webxr_spaces.hpp>

#include "./xr_hand.hpp"
#include "./xr_space.hpp"

using namespace std;
using namespace v8;

namespace endor
{
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
        InstanceMethod(isolate, prototype, Symbol::GetIterator(isolate), &XRHand::Values);
      }

      // static
      Local<Object> XRHand::NewInstance(Isolate *isolate, const client_xr::XRHand &hand)
      {
        return XRHandBase::NewInstance(isolate, make_shared<client_xr::XRHand>(hand));
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
        Local<Context> context = isolate->GetCurrentContext();
        Local<Array> entries = Array::New(isolate);

        const auto &joints = handle()->values();
        uint32_t index = 0;

        for (const auto &joint : joints)
        {
          Local<Array> entry = Array::New(isolate, 2);
          {
            entry->Set(context, 0, String::NewFromUtf8(isolate, joint->name.c_str()).ToLocalChecked()).Check();
            entry->Set(context, 1, XRSpace::GetOrNewInstance(isolate, joint)).Check();
          }
          entries->Set(context, index++, entry).Check();
        }
        info.GetReturnValue().Set(entries);
      }

      void XRHand::ForEach(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1 || !info[0]->IsFunction())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "forEach", "Requires a callback function as argument")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        Local<Function> callback = info[0].As<Function>();

        const auto &joints = handle()->values();
        for (const auto &joint : joints)
        {
          Local<Value> args[2];
          args[0] = XRSpace::GetOrNewInstance(isolate, joint);
          args[1] = String::NewFromUtf8(isolate, joint->name.c_str()).ToLocalChecked();
          callback->Call(context, info.This(), 2, args).ToLocalChecked();
        }
        info.GetReturnValue().SetUndefined();
      }

      void XRHand::Get(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        if (info.Length() < 1 || !info[0]->IsString())
        {
          isolate->ThrowException(Exception::TypeError(
            MakeMethodError(isolate, "get", "Requires a string key as argument")));
          return;
        }

        Local<Context> context = isolate->GetCurrentContext();
        String::Utf8Value key(isolate, info[0]);

        auto joint = handle()->get(*key);
        if (joint == nullptr)
          info.GetReturnValue().SetNull();
        else
          info.GetReturnValue().Set(XRSpace::GetOrNewInstance(isolate, joint));
      }

      void XRHand::Keys(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();
        Local<Array> keys = Array::New(isolate);

        uint32_t index = 0;
        for (const auto &name : handle()->keys())
        {
          keys->Set(context,
                    index++,
                    String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked())
            .Check();
        }
        info.GetReturnValue().Set(keys);
      }

      void XRHand::Values(const FunctionCallbackInfo<Value> &info)
      {
        Isolate *isolate = info.GetIsolate();
        HandleScope scope(isolate);

        Local<Context> context = isolate->GetCurrentContext();
        Local<Array> values = Array::New(isolate);

        const auto &joints = handle()->values();
        uint32_t index = 0;
        for (const auto &joint : joints)
        {
          values->Set(context,
                      index++,
                      XRSpace::GetOrNewInstance(isolate, joint))
            .Check();
        }
        info.GetReturnValue().Set(values);
      }
    }
  }
} // namespace endor
