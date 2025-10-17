#include "./response.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  void Response::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    HandleScope scope(isolate);
    Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

    InstanceReadonlyPropertyAccessor(isolate, prototype, "body", &Response::BodyGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "bodyUsed", &Response::BodyUsedGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "headers", &Response::HeadersGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "ok", &Response::OkGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "redirected", &Response::RedirectedGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "status", &Response::StatusGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "statusText", &Response::StatusTextGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "type", &Response::TypeGetter);
    InstanceReadonlyPropertyAccessor(isolate, prototype, "url", &Response::UrlGetter);

    InstanceMethod(isolate, prototype, "arrayBuffer", &Response::ArrayBuffer);
    InstanceMethod(isolate, prototype, "blob", &Response::Blob);
    InstanceMethod(isolate, prototype, "bytes", &Response::Bytes);
    InstanceMethod(isolate, prototype, "clone", &Response::Clone);
    InstanceMethod(isolate, prototype, "formData", &Response::FormData);
    InstanceMethod(isolate, prototype, "json", &Response::Json);
    InstanceMethod(isolate, prototype, "text", &Response::Text);
  }

  // static
  Local<Object> Response::NewInstance(Isolate *isolate, Local<Value> source_response)
  {
    EscapableHandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();
    Local<Object> instance = ResponseBase::NewInstance(isolate, nullptr);
    Response *response = Unwrap(isolate, instance);
    assert(response != nullptr && "Response instance should be valid.");
    assert(source_response->IsObject() && "Source response must be an object.");

    response->source_response_handle_.Reset(isolate,
                                            source_response->ToObject(context).ToLocalChecked());
    return scope.Escape(instance);
  }

  void Response::SourcePropertyGetter(const char *name, const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (source_response_handle_.IsEmpty())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, name, "Source response is not available.")));
      return;
    }

    Local<Object> source_response = source_response_handle_.Get(isolate);
    Local<String> name_string = String::NewFromUtf8(isolate, name).ToLocalChecked();
    return args.GetReturnValue().Set(
      source_response->Get(context, name_string).ToLocalChecked());
  }

  void Response::SourceMethodCall(const char *name, const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (source_response_handle_.IsEmpty())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, name, "Source response is not available.")));
      return;
    }

    Local<Object> source_response = source_response_handle_.Get(isolate);
    Local<String> name_string = String::NewFromUtf8(isolate, name).ToLocalChecked();
    Local<Function> func = source_response->Get(context, name_string)
                             .ToLocalChecked()
                             .As<Function>();

    Local<Value> result_value = func->Call(context, source_response, 0, nullptr).ToLocalChecked();
    args.GetReturnValue().Set(result_value);
  }

  void Response::BodyGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("body", args);
  }

  void Response::BodyUsedGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("bodyUsed", args);
  }

  void Response::HeadersGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("headers", args);
  }

  void Response::OkGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("ok", args);
  }

  void Response::RedirectedGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("redirected", args);
  }

  void Response::StatusGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("status", args);
  }

  void Response::StatusTextGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("statusText", args);
  }

  void Response::TypeGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("type", args);
  }

  void Response::UrlGetter(const FunctionCallbackInfo<Value> &args)
  {
    SourcePropertyGetter("url", args);
  }

  void Response::ArrayBuffer(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    if (source_response_handle_.IsEmpty())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "arrayBuffer", "Source response is not available.")));
      return;
    }

    Local<Object> source_response = source_response_handle_.Get(isolate);
    Local<String> name_string = String::NewFromUtf8(isolate, "arrayBuffer").ToLocalChecked();
    Local<Function> func = source_response->Get(context, name_string)
                             .ToLocalChecked()
                             .As<Function>();

    Local<Value> result_value = func->Call(context, source_response, 0, nullptr).ToLocalChecked();
    if (!result_value->IsPromise())
    {
      isolate->ThrowException(Exception::TypeError(
        MakeMethodError(isolate, "arrayBuffer", "Source response.arrayBuffer() did not return a Promise.")));
      return;
    }

    Local<Promise> result_promise = result_value.As<Promise>();
    auto OnResolve = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      Local<Value> result_value;
      {
        // Serialize and deserialize the ArrayBuffer to transfer it safely.
        ValueSerializer serializer(isolate);
        serializer.WriteHeader();
        serializer.WriteValue(context, args[0]).ToChecked();
        auto buffer = serializer.Release();

        ValueDeserializer deserializer(isolate, buffer.first, buffer.second);
        assert(deserializer.ReadHeader(context).ToChecked() && "Failed to read serialized ArrayBuffer header.");
        if (!deserializer.ReadValue(context).ToLocal(&result_value)) [[unlikely]]
        {
          result_value = Local<Value>();
        }
      }

      if (result_value.IsEmpty())
      {
        resolver->Reject(context,
                         Exception::Error(
                           MakeMethodError(isolate, "Response.arrayBuffer", "Failed to serialize ArrayBuffer")))
          .ToChecked();
        return;
      }
      else
      {
        resolver->Resolve(context, result_value).ToChecked();
      }
    };
    auto OnReject = [](const FunctionCallbackInfo<Value> &args)
    {
      Isolate *isolate = args.GetIsolate();
      HandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      Local<Promise::Resolver> resolver = args.Data().As<Promise::Resolver>();
      Local<Value> error = args[0];
      resolver->Reject(context, error).ToChecked();
    };

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    result_promise->Then(context,
                         Function::New(context, OnResolve, resolver).ToLocalChecked(),
                         Function::New(context, OnReject, resolver).ToLocalChecked())
      .ToLocalChecked();

    // Return the promise
    args.GetReturnValue().Set(resolver->GetPromise());
  }

  void Response::Blob(const FunctionCallbackInfo<Value> &args)
  {
    SourceMethodCall("blob", args);
  }

  void Response::Bytes(const FunctionCallbackInfo<Value> &args)
  {
    SourceMethodCall("bytes", args);
  }

  void Response::Clone(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    isolate->ThrowException(Exception::TypeError(
      MakeMethodError(isolate, "clone", "Not implemented.")));
  }

  void Response::FormData(const FunctionCallbackInfo<Value> &args)
  {
    SourceMethodCall("formData", args);
  }

  void Response::Json(const FunctionCallbackInfo<Value> &args)
  {
    SourceMethodCall("json", args);
  }

  void Response::Text(const FunctionCallbackInfo<Value> &args)
  {
    SourceMethodCall("text", args);
  }
}
