#include <memory>
#include <client/script_bindings/url/url.hpp>
#include <client/script_bindings/url/url_search_params.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::url_bindings
{
  // static
  void URL::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    InstancePropertyAccessor(isolate, prototype, "hash", &URL::HashGetter, &URL::HashSetter);
    InstancePropertyAccessor(isolate, prototype, "host", &URL::HostGetter, &URL::HostSetter);
    InstancePropertyAccessor(isolate, prototype, "hostname", &URL::HostnameGetter, &URL::HostnameSetter);
    InstancePropertyAccessor(isolate, prototype, "href", &URL::HrefGetter, &URL::HrefSetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "origin", &URL::OriginGetter);
    InstancePropertyAccessor(isolate, prototype, "pathname", &URL::PathnameGetter, &URL::PathnameSetter);
    InstancePropertyAccessor(isolate, prototype, "port", &URL::PortGetter, &URL::PortSetter);
    InstancePropertyAccessor(isolate, prototype, "protocol", &URL::ProtocolGetter, &URL::ProtocolSetter);
    InstancePropertyAccessor(isolate, prototype, "search", &URL::SearchGetter, &URL::SearchSetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "searchParams", &URL::SearchParamsGetter);
    InstancePropertyAccessor(isolate, prototype, "username", &URL::UsernameGetter, &URL::UsernameSetter);
    InstancePropertyAccessor(isolate, prototype, "password", &URL::PasswordGetter, &URL::PasswordSetter);

    InstanceMethod(isolate, prototype, "toString", &URL::ToString);
    InstanceMethod(isolate, prototype, "toJSON", &URL::ToJSON);
  }

  URL::URL(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
      : URLBase(isolate, args)
  {
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeConstructorError(isolate, "Requires at least 1 argument")));
      return;
    }

    string url;
    string base;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      if (utf8Value.length() == 0)
      {
        isolate->ThrowException(Exception::TypeError(
          MakeConstructorError(isolate, "Invalid URL string")));
        return;
      }
      url = string(*utf8Value);
    }
    if (args.Length() >= 2 && args[1]->IsString())
    {
      String::Utf8Value utf8Value(isolate, args[1]);
      base = string(*utf8Value);
    }

    shared_ptr<client_url::URL> urlData;
    try
    {
      urlData = make_shared<client_url::URL>(url, base);
    }
    catch (const exception &e)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeConstructorError(isolate, e.what())));
      return;
    }
    setData(urlData);
  }

  void URL::HashGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->hash.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::HashSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->hash = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::HostGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->host.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::HostSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->host = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::HostnameGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->hostname.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::HostnameSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->hostname = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::HrefGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->href().c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::HrefSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->setHref(value);
    args.GetReturnValue().SetUndefined();
  }

  void URL::OriginGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->origin().c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::PathnameGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->pathname.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::PathnameSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->pathname = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::PortGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->port.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::PortSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->port = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::ProtocolGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->protocol.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::ProtocolSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->protocol = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::SearchGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->search.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::SearchSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->search = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::SearchParamsGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    auto searchParams = make_shared<client_url::URLSearchParams>(handle()->searchParams());
    args.GetReturnValue().Set(URLSearchParams::NewInstance(isolate, searchParams));
  }

  void URL::UsernameGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->username.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::UsernameSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->username = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::PasswordGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->password.c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::PasswordSetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string value;
    {
      String::Utf8Value utf8Value(isolate, args[0]);
      value = string(*utf8Value);
    }
    handle()->password = value;
    args.GetReturnValue().SetUndefined();
  }

  void URL::ToString(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->href().c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }

  void URL::ToJSON(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<String> value = String::NewFromUtf8(isolate, handle()->href().c_str()).ToLocalChecked();
    args.GetReturnValue().Set(value);
  }
}
