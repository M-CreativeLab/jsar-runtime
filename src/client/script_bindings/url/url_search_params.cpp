#include <memory>
#include <client/script_bindings/url/url_search_params.hpp>

using namespace std;
using namespace v8;

namespace script_bindings::url_bindings
{
  // static
  Local<Function> URLSearchParams::Initialize(Isolate *isolate)
  {
    EscapableHandleScope scope(isolate);
    URLSearchParamsIterator::Initialize(isolate);
    return scope.Escape(URLSearchParamsBase::Initialize(isolate));
  }

  // static
  void URLSearchParams::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
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
    InstanceMethod(isolate, prototype, Symbol::GetIterator(isolate), &URLSearchParams::GetIterator);
  }

  URLSearchParams::URLSearchParams(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::SizeGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    args.GetReturnValue().Set(Integer::New(isolate,
                                           static_cast<int>(handle()->size())));
  }

  void URLSearchParams::Append(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (args.Length() < 2)
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodArgCountError(isolate, "append", 2, args.Length())));
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

  void URLSearchParams::Delete(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::Get(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::GetAll(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::Has(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::Set(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::ToString(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    string result = handle()->toString();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                  result.c_str())
                                .ToLocalChecked());
  }

  void URLSearchParams::Keys(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::Values(const FunctionCallbackInfo<Value> &args)
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

  void URLSearchParams::GetIterator(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);


    args.GetReturnValue().Set(URLSearchParamsIterator::NewInstance(isolate, this));
  }

  // static
  void URLSearchParamsIterator::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    InstanceMethod(isolate, prototype, "next", &URLSearchParamsIterator::Next);
    InstanceMethod(isolate, prototype, "return", &URLSearchParamsIterator::Return);
    InstanceMethod(isolate, prototype, "throw", &URLSearchParamsIterator::Throw);
  }

  // static
  Local<Object> URLSearchParamsIterator::NewInstance(Isolate *isolate, URLSearchParams *params)
  {
    EscapableHandleScope scope(isolate);
    auto obj = URLSearchParamsIteratorBase::NewInstance(isolate, nullptr);
    auto iterator = Unwrap(isolate, obj);
    assert(iterator != nullptr && "iterator must not be null");
    iterator->params_handle_.Reset(isolate, params->value());
    return scope.Escape(obj);
  }

  void URLSearchParamsIterator::Next(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Object> params_object = params_handle_.Get(isolate).As<Object>();
    auto params_binding = URLSearchParams::Unwrap(isolate, params_object);
    assert(params_binding != nullptr && "params_binding must not be null");
    auto &params = *(params_binding->handle());

    Local<Object> iteratorResult = Object::New(isolate);
    if (current_index_ == params.size())
    {
      iteratorResult->Set(context,
                          String::NewFromUtf8Literal(isolate, "done"),
                          Boolean::New(isolate, true))
        .FromJust();
      iteratorResult->Set(context,
                          String::NewFromUtf8Literal(isolate, "value"),
                          Undefined(isolate))
        .FromJust();
    }
    else
    {
      Local<Object> value = Array::New(isolate);
      {
        const auto &param = params.at(current_index_);
        value->Set(context, 0, String::NewFromUtf8(isolate, param.first.c_str()).ToLocalChecked()).ToChecked();
        value->Set(context, 1, String::NewFromUtf8(isolate, param.second.c_str()).ToLocalChecked()).ToChecked();
        current_index_++;
      }

      iteratorResult->Set(context,
                          String::NewFromUtf8Literal(isolate, "done"),
                          Boolean::New(isolate, false))
        .FromJust();
      iteratorResult->Set(context,
                          String::NewFromUtf8Literal(isolate, "value"),
                          value)
        .FromJust();
    }

    // Return the iterator result object
    args.GetReturnValue().Set(iteratorResult);
  }

  void URLSearchParamsIterator::Return(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "return", "Not implemented yet.")));
  }

  void URLSearchParamsIterator::Throw(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "throw", "Not implemented yet.")));
  }
}
