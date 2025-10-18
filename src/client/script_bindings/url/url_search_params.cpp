#include <memory>
#include <client/script_bindings/url/url_search_params.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::url_bindings
{
  // static
  void URLSearchParams::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    InstanceReadonlyPropertyAccessor(isolate, prototype, "size", &URLSearchParams::SizeGetter);
    InstanceMethod(isolate, prototype, "append", &URLSearchParams::Append);
    InstanceMethod(isolate, prototype, "delete", &URLSearchParams::Delete);
    InstanceMethod(isolate, prototype, "get", &URLSearchParams::Get);
    InstanceMethod(isolate, prototype, "getAll", &URLSearchParams::GetAll);
    InstanceMethod(isolate, prototype, "has", &URLSearchParams::Has);
    InstanceMethod(isolate, prototype, "set", &URLSearchParams::Set);
    InstanceMethod(isolate, prototype, "toString", &URLSearchParams::ToString);
    InstanceMethod(isolate, prototype, "keys", &URLSearchParams::Keys);
    InstanceMethod(isolate, prototype, "values", &URLSearchParams::Values);
  }

  URLSearchParams::URLSearchParams(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
      : URLSearchParamsBase(isolate, args)
  {
    HandleScope scope(isolate);

    if (args.Length() == 0)
    {
      setData(make_shared<client_url::URLSearchParams>());
    }
    else if (args[0]->IsString())
    {
      String::Utf8Value query_utf8(isolate, args[0]);
      string queryStr(*query_utf8, query_utf8.length());
      setData(make_shared<client_url::URLSearchParams>(queryStr));
    }
    else
    {
      isolate->ThrowException(Exception::TypeError(
        MakeConstructorError(isolate, "Unsupported argument type for URLSearchParams constructor.")));
      return;
    }
  }

  void URLSearchParams::SizeGetter(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(Integer::New(isolate,
                                           static_cast<int>(handle()->size())));
  }

  void URLSearchParams::Append(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.append", 2, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }
    string value;
    {
      String::Utf8Value utf8(isolate, args[1]);
      value = string(*utf8, utf8.length());
    }

    handle()->append(name, value);
    args.GetReturnValue().SetUndefined();
  }

  void URLSearchParams::Delete(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.delete", 1, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }
    optional<string> value = nullopt;
    if (args.Length() >= 2)
    {
      String::Utf8Value utf8(isolate, args[1]);
      value = string(*utf8, utf8.length());
    }

    handle()->remove(name, value);
    args.GetReturnValue().SetUndefined();
  }

  void URLSearchParams::Get(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.get", 1, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }

    optional<string> result = handle()->get(name);
    if (result.has_value())
    {
      args.GetReturnValue().Set(String::NewFromUtf8(isolate, result->c_str()).ToLocalChecked());
    }
    else
    {
      args.GetReturnValue().Set(Null(isolate));
    }
  }

  void URLSearchParams::GetAll(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.getAll", 1, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }

    const vector<string> &results = handle()->getAll(name);
    Local<Array> array = Array::New(isolate, static_cast<int>(results.size()));
    for (size_t i = 0; i < results.size(); ++i)
    {
      array->Set(context,
                 static_cast<uint32_t>(i),
                 String::NewFromUtf8(isolate, results[i].c_str()).ToLocalChecked())
        .Check();
    }
    args.GetReturnValue().Set(array);
  }

  void URLSearchParams::Has(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 1)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.has", 1, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }

    bool result = handle()->has(name);
    args.GetReturnValue().Set(Boolean::New(isolate, result));
  }

  void URLSearchParams::Set(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "URLSearchParams.set", 2, args.Length())));
      return;
    }

    string name;
    {
      String::Utf8Value utf8(isolate, args[0]);
      name = string(*utf8, utf8.length());
    }
    string value;
    {
      String::Utf8Value utf8(isolate, args[1]);
      value = string(*utf8, utf8.length());
    }

    handle()->set(name, value);
    args.GetReturnValue().SetUndefined();
  }

  void URLSearchParams::ToString(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string result = handle()->toString();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  result.c_str())
                                .ToLocalChecked());
  }

  void URLSearchParams::Keys(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    const vector<string> &keys = handle()->keys();
    Local<Array> array = Array::New(isolate, static_cast<int>(keys.size()));
    for (size_t i = 0; i < keys.size(); ++i)
    {
      array->Set(context,
                 static_cast<uint32_t>(i),
                 String::NewFromUtf8(isolate, keys[i].c_str()).ToLocalChecked())
        .Check();
    }
    args.GetReturnValue().Set(array);
  }

  void URLSearchParams::Values(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    const vector<string> &values = handle()->values();
    Local<Array> array = Array::New(isolate, static_cast<int>(values.size()));
    for (size_t i = 0; i < values.size(); ++i)
    {
      array->Set(context,
                 static_cast<uint32_t>(i),
                 String::NewFromUtf8(isolate, values[i].c_str()).ToLocalChecked())
        .Check();
    }
    args.GetReturnValue().Set(array);
  }
}
