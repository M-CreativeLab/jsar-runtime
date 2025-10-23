#include "./location.hpp"

namespace endor
{
  namespace script_bindings
  {
    using namespace std;
    using namespace v8;

    void Location::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Properties
      InstanceAccessor(isolate, instanceTemplate, "href", &Location::HrefGetter, &Location::HrefSetter);
      InstanceAccessor(isolate, instanceTemplate, "protocol", &Location::ProtocolGetter, &Location::ProtocolSetter);
      InstanceAccessor(isolate, instanceTemplate, "host", &Location::HostGetter, &Location::HostSetter);
      InstanceAccessor(isolate, instanceTemplate, "hostname", &Location::HostnameGetter, &Location::HostnameSetter);
      InstanceAccessor(isolate, instanceTemplate, "port", &Location::PortGetter, &Location::PortSetter);
      InstanceAccessor(isolate, instanceTemplate, "pathname", &Location::PathnameGetter, &Location::PathnameSetter);
      InstanceAccessor(isolate, instanceTemplate, "search", &Location::SearchGetter, &Location::SearchSetter);
      InstanceAccessor(isolate, instanceTemplate, "hash", &Location::HashGetter, &Location::HashSetter);
      InstanceAccessor(isolate, instanceTemplate, "origin", &Location::OriginGetter, nullptr);

      // Methods
      InstanceMethod(isolate, instanceTemplate, "assign", &Location::Assign);
      InstanceMethod(isolate, instanceTemplate, "reload", &Location::Reload);
      InstanceMethod(isolate, instanceTemplate, "replace", &Location::Replace);
      InstanceMethod(isolate, instanceTemplate, "toString", &Location::ToString);
    }

    Local<Object> Location::NewInstance(Isolate *isolate, std::shared_ptr<browser::Location> nativeLocation)
    {
      EscapableHandleScope scope(isolate);
      return scope.Escape(LocationBase::NewInstance(isolate, nativeLocation).As<Object>());
    }

    Local<Object> Location::NewInstance(Isolate *isolate, const string &url)
    {
      return NewInstance(isolate, make_shared<browser::Location>(url));
    }

    // Instance accessors
    void Location::HrefGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->href.c_str()).ToLocalChecked());
    }

    void Location::HrefSetter(Local<Value> value,
                              const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
    }

    void Location::ProtocolGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->protocol.c_str()).ToLocalChecked());
    }

    void Location::ProtocolSetter(Local<Value> value,
                                  const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::HostGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->host.c_str()).ToLocalChecked());
    }

    void Location::HostSetter(Local<Value> value,
                              const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::HostnameGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->hostname.c_str()).ToLocalChecked());
    }

    void Location::HostnameSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::PortGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    to_string(handle()->port).c_str())
                                  .ToLocalChecked());
    }

    void Location::PortSetter(Local<Value> value,
                              const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::PathnameGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->pathname.c_str()).ToLocalChecked());
    }

    void Location::PathnameSetter(Local<Value> value,
                                  const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::SearchGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->search.c_str()).ToLocalChecked());
    }

    void Location::SearchSetter(Local<Value> value,
                                const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::HashGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->hash.c_str()).ToLocalChecked());
    }

    void Location::HashSetter(Local<Value> value,
                              const PropertyCallbackInfo<void> &info)
    {
    }

    void Location::OriginGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->origin.c_str()).ToLocalChecked());
    }

    void Location::Assign(const FunctionCallbackInfo<Value> &info)
    {
      if (info.Length() != 1 || !info[0]->IsString())
      {
        info.GetIsolate()->ThrowException(
          String::NewFromUtf8(info.GetIsolate(), "Invalid argument(s) to Location.assign").ToLocalChecked());
        return;
      }
      return;
    }

    void Location::Reload(const FunctionCallbackInfo<Value> &info)
    {
      if (info.Length() != 0)
      {
        info.GetIsolate()->ThrowException(
          String::NewFromUtf8(info.GetIsolate(), "Location.reload doesn't accept any arguments").ToLocalChecked());
        return;
      }
    }

    void Location::Replace(const FunctionCallbackInfo<Value> &info)
    {
      if (info.Length() != 1 || !info[0]->IsString())
      {
        info.GetIsolate()->ThrowException(
          String::NewFromUtf8(info.GetIsolate(), "Invalid argument(s) to Location.replace").ToLocalChecked());
        return;
      }
    }

    void Location::ToString(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);
      info.GetReturnValue().Set(String::NewFromUtf8(isolate, handle()->href.c_str()).ToLocalChecked());
    }
  }
} // namespace endor
