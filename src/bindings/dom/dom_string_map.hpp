#pragma once

#include <map>
#include <memory>
#include <string>

#include <node/v8.h>
#include <node/node_api.h>
#include <napi.h>

namespace dombinding
{
  class DOMStringMap
  {
  public:
    template <typename InstanceType>
      requires std::is_base_of_v<DOMStringMap, InstanceType>
    static Napi::Value NewInstance(napi_env env, InstanceType *instance)
    {
      using namespace v8;

      Napi::EscapableHandleScope scope(env);

      Isolate *isolate = Isolate::GetCurrent();
      Local<Context> context = isolate->GetCurrentContext();

      Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
      tpl->InstanceTemplate()->SetInternalFieldCount(1);

      tpl->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(
        DOMStringMap::PropertyGetter<InstanceType>,  // Getter
        DOMStringMap::PropertySetter<InstanceType>,  // Setter
        nullptr,                                     // Query
        DOMStringMap::PropertyDeleter<InstanceType>, // Deleter
        nullptr,                                     // Enumerator
        nullptr,                                     // Definer
        nullptr,                                     // Descriptor
        Local<Value>(),
        PropertyHandlerFlags::kNone));

      Local<Function> constructor = tpl->GetFunction(context).ToLocalChecked();
      constructor->SetName(String::NewFromUtf8(isolate, "DOMStringMap").ToLocalChecked());

      Local<Object> jsInstance = constructor->NewInstance(context).ToLocalChecked();
      jsInstance->SetInternalField(0, External::New(isolate, instance));

      auto finalizer = [](const WeakCallbackInfo<InstanceType> &info)
      {
        InstanceType *wrapper = info.GetParameter();
        if (wrapper != nullptr)
        {
          wrapper->persistentHandle.Reset();
          delete wrapper;
        }
      };
      instance->persistentHandle.Reset(isolate, jsInstance);
      instance->persistentHandle.SetWeak(instance, finalizer, WeakCallbackType::kParameter);

      napi_value napiValue;
      memcpy(&napiValue, &jsInstance, sizeof(jsInstance));
      return scope.Escape(Napi::Value(env, napiValue));
    }

    template <typename InstanceType>
      requires std::is_base_of_v<DOMStringMap, InstanceType>
    static void PropertyGetter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto isolate = info.GetIsolate();
      v8::String::Utf8Value property_utf8(isolate, property);

      auto holder = info.Holder();
      auto externalValue = holder->GetInternalField(0);
      if (externalValue.IsEmpty())
      {
        info.GetReturnValue().Set(Undefined(isolate));
        return;
      }

      InstanceType *instance = static_cast<InstanceType *>(v8::Local<v8::External>::Cast(externalValue)->Value());
      if (!instance)
      {
        info.GetReturnValue().Set(Undefined(isolate));
        return;
      }

      auto value = instance->get(*property_utf8);
      if (value.has_value())
      {
        info.GetReturnValue().Set(
          v8::String::NewFromUtf8(isolate, value->c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(Undefined(isolate));
      }
    }

    template <typename InstanceType>
      requires std::is_base_of_v<DOMStringMap, InstanceType>
    static void PropertySetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      v8::Isolate *isolate = info.GetIsolate();
      v8::Local<v8::Context> context = isolate->GetCurrentContext();

      auto holder = info.Holder();
      auto externalValue = holder->GetInternalField(0);
      if (externalValue.IsEmpty())
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8Literal(isolate, "Internal error: missing instance")));
        return;
      }

      InstanceType *instance = static_cast<InstanceType *>(v8::Local<v8::External>::Cast(externalValue)->Value());
      if (!instance)
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8Literal(isolate, "Internal error: instance is null")));
        return;
      }

      v8::String::Utf8Value key_utf8(isolate, property);
      std::string key(*key_utf8);

      v8::MaybeLocal<v8::String> maybe_str = value->ToString(context);
      if (maybe_str.IsEmpty())
      {
        isolate->ThrowException(v8::Exception::TypeError(
          v8::String::NewFromUtf8Literal(isolate, "Value must be a string")));
        return;
      }
      v8::Local<v8::String> str = maybe_str.ToLocalChecked();
      v8::String::Utf8Value value_utf8(isolate, str);
      std::string value_str(*value_utf8);

      instance->set(key, value_str);
      info.GetReturnValue().Set(value);
    }

    template <typename InstanceType>
      requires std::is_base_of_v<DOMStringMap, InstanceType>
    static void PropertyDeleter(v8::Local<v8::Name> property, const v8::PropertyCallbackInfo<v8::Boolean> &info)
    {
      auto isolate = info.GetIsolate();
      v8::String::Utf8Value property_utf8(isolate, property);

      auto holder = info.Holder();
      auto externalValue = holder->GetInternalField(0);
      if (externalValue.IsEmpty())
      {
        info.GetReturnValue().Set(false);
        return;
      }

      InstanceType *instance = static_cast<InstanceType *>(v8::Local<v8::External>::Cast(externalValue)->Value());
      if (!instance)
      {
        info.GetReturnValue().Set(false);
        return;
      }

      instance->unset(*property_utf8);
      info.GetReturnValue().Set(true);
    }

  public:
    virtual ~DOMStringMap() = default;

  public:
    virtual std::optional<std::string> get(const std::string &key) const = 0;
    virtual void set(const std::string &key, const std::string &value) = 0;
    virtual void unset(const std::string &key) = 0;

  private:
    v8::Persistent<v8::Object> persistentHandle;
  };
}
