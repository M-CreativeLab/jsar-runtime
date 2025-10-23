#include <client/script_bindings/fetch/response.hpp>
#include <client/script_bindings/fileapi/blob.hpp>

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

  // static
  Local<Object> Response::NewInstance(Isolate *isolate, shared_ptr<client_fileapi::Blob> blob)
  {
    EscapableHandleScope scope(isolate);
    auto instance = ResponseBase::NewInstance(isolate,
                                              make_shared<client_fetch::Response>(blob));
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

  void Response::SourceMethodCall(const char *name,
                                  const FunctionCallbackInfo<Value> &args,
                                  Local<Promise::Resolver> resolver)
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
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      cerr << "Warning: Response.body is not available." << endl;
      args.GetReturnValue().SetNull();
    }
    else
    {
      SourcePropertyGetter("body", args);
    }
  }

  void Response::BodyUsedGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(Boolean::New(isolate, handle()->bodyUsed()));
    }
    else
    {
      SourcePropertyGetter("bodyUsed", args);
    }
  }

  void Response::HeadersGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      // TODO(yorkie): Implement Headers object creation from C++ side.
      args.GetReturnValue().SetNull();
    }
    else
    {
      SourcePropertyGetter("headers", args);
    }
  }

  void Response::OkGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(Boolean::New(isolate, handle()->ok()));
    }
    else
    {
      SourcePropertyGetter("ok", args);
    }
  }

  void Response::RedirectedGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(Boolean::New(isolate, handle()->redirected()));
    }
    else
    {
      SourcePropertyGetter("redirected", args);
    }
  }

  void Response::StatusGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(Integer::New(isolate, handle()->status()));
    }
    else
    {
      SourcePropertyGetter("status", args);
    }
  }

  void Response::StatusTextGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->statusText().c_str())
                                  .ToLocalChecked());
      return;
    }
    else
    {
      SourcePropertyGetter("statusText", args);
    }
  }

  void Response::TypeGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      switch (handle()->type())
      {
      case client_fetch::Response::kBasic:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "basic").ToLocalChecked());
        break;
      case client_fetch::Response::kCORS:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "cors").ToLocalChecked());
        break;
      case client_fetch::Response::kDefault:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "default").ToLocalChecked());
        break;
      case client_fetch::Response::kError:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "error").ToLocalChecked());
        break;
      case client_fetch::Response::kOpaque:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "opaque").ToLocalChecked());
        break;
      case client_fetch::Response::kOpaqueRedirect:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "opaqueredirect").ToLocalChecked());
        break;
      default:
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, "default").ToLocalChecked());
        break;
      }
    }
    else
    {
      SourcePropertyGetter("type", args);
    }
  }

  void Response::UrlGetter(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);

    if (hasData())
    {
      args.GetReturnValue().Set(String::NewFromUtf8(isolate,
                                                    handle()->url().c_str())
                                  .ToLocalChecked());
      return;
    }
    else
    {
      SourcePropertyGetter("url", args);
    }
  }

  void Response::ArrayBuffer(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    // If the Response has data from the C++ side, return it directly.
    if (hasData())
    {
      // If the Response has data from the C++ side, create an ArrayBuffer from it.
      const auto &body_bytes = handle()->body();
      auto backing_store = ArrayBuffer::NewBackingStore(
        const_cast<uint8_t *>(body_bytes.data()),
        body_bytes.size(),
        [](void *data, size_t length, void *deleter_data)
        {
          // No-op deleter since the data is owned by the Response
        },
        nullptr);
      auto arraybuffer_value = ArrayBuffer::New(isolate, move(backing_store));
      resolver->Resolve(context, arraybuffer_value).ToChecked();
    }
    else
    {
      // Otherwise, call the source response's arrayBuffer() method.

      // Check if source response is available
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

      // Link the fetch promise to the returned resolver
      result_promise->Then(context,
                           Function::New(context, OnResolve, resolver).ToLocalChecked(),
                           Function::New(context, OnReject, resolver).ToLocalChecked())
        .ToLocalChecked();
    }
  }

  void Response::Blob(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    if (hasData())
    {
      auto blob = make_shared<client_fileapi::Blob>(handle()->body());
      resolver->Resolve(context, fileapi_bindings::Blob::NewInstance(isolate, blob)).ToChecked();
    }
    else
    {
      SourceMethodCall("blob", args, resolver);
    }
  }

  void Response::Bytes(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    if (hasData())
    {
      const auto &body_bytes = handle()->body();
      auto backing_store = ArrayBuffer::NewBackingStore(
        const_cast<uint8_t *>(body_bytes.data()),
        body_bytes.size(),
        [](void *data, size_t length, void *deleter_data)
        {
          // No-op deleter since the data is owned by the Response
        },
        nullptr);
      auto arraybuffer_value = ArrayBuffer::New(isolate, move(backing_store));
      auto uint8_array = Uint8Array::New(arraybuffer_value, 0, body_bytes.size());
      resolver->Resolve(context, uint8_array).ToChecked();
    }
    else
    {
      SourceMethodCall("bytes", args, resolver);
    }
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
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    if (hasData())
    {
      resolver->Reject(context,
                       Exception::Error(
                         MakeMethodError(isolate, "formData", "Not implemented for C++ Response.")))
        .ToChecked();
    }
    else
    {
      SourceMethodCall("formData", args, resolver);
    }
  }

  void Response::Json(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    if (hasData())
    {
      const auto &body_bytes = handle()->body();
      string text(body_bytes.begin(), body_bytes.end());

      Local<Value> json_value;
      MaybeLocal<Value> maybe_json_value =
        JSON::Parse(context, String::NewFromUtf8(isolate, text.c_str()).ToLocalChecked());
      if (!maybe_json_value.ToLocal(&json_value)) [[unlikely]]
      {
        resolver->Reject(
                  context,
                  Exception::Error(
                    MakeMethodError(isolate, "json", "Failed to parse JSON from response body.")))
          .ToChecked();
      }
      else
      {
        resolver->Resolve(context, json_value).ToChecked();
      }
    }
    else
    {
      SourceMethodCall("json", args, resolver);
    }
  }

  void Response::Text(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Context> context = isolate->GetCurrentContext();

    Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
    args.GetReturnValue().Set(resolver->GetPromise());

    if (hasData())
    {
      const auto &body_bytes = handle()->body();
      string text(body_bytes.begin(), body_bytes.end());
      resolver->Resolve(context,
                        String::NewFromUtf8(isolate, text.c_str()).ToLocalChecked())
        .ToChecked();
      return;
    }
    else
    {
      SourceMethodCall("text", args, resolver);
    }
  }
}
