#include <node/v8.h>
#include "./runtime_context.hpp"

namespace dom
{
  using namespace std;

  void RuntimeContext::initialize()
  {
    scriptingContext = make_shared<DOMScriptingContext>(getSharedPtr());
  }

  void RuntimeContext::setBaseURI(const string newBaseURI)
  {
    baseURI = newBaseURI;
  }

  void ResolveResource(const v8::FunctionCallbackInfo<v8::Value> &info)
  {
    auto isolate = info.GetIsolate();
    auto context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);
    v8::HandleScope handleScope(isolate);
    {
      auto callback = reinterpret_cast<FunctionCallback *>(info.Data().As<v8::External>()->Value());
      (*callback)(info);
      delete callback;
    }
  }

  void RuntimeContext::setResourceLoaderValue(v8::Local<v8::Value> value)
  {
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    {
      resourceLoaderValue.Reset(isolate, value.As<v8::Object>());
      assert(!resourceLoaderValue.IsEmpty());
    }
  }

  void RuntimeContext::fetchResource(const string &url,
                                     const string &responseType,
                                     const FunctionCallback &responseCallback,
                                     const optional<FunctionCallback> errorCallback)
  {
    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> promiseValue = fetchResourceInternal(url, responseType);
    if (!promiseValue->IsPromise())
      return;

    v8::Local<v8::Promise> fetchPromise = promiseValue.As<v8::Promise>();
    v8::Local<v8::External> resolveCallbackExternal = v8::External::New(isolate, new FunctionCallback(responseCallback));
    auto resolve = v8::Function::New(context, ResolveResource, resolveCallbackExternal);

    // Schedule the callbacks
    if (errorCallback.has_value())
    {
      v8::Local<v8::External> rejectCallbackExternal = v8::External::New(isolate, new FunctionCallback(errorCallback.value()));
      auto reject = v8::Function::New(context, ResolveResource, rejectCallbackExternal);
      fetchPromise->Then(context, resolve.ToLocalChecked(), reject.ToLocalChecked())
        .ToLocalChecked();
    }
    else
    {
      fetchPromise->Then(context, resolve.ToLocalChecked())
        .ToLocalChecked();
    }
  }

  v8::Local<v8::Value> RuntimeContext::fetchResourceSync(const string &url, const string &responseType)
  {
    assert(false && "fetchResourceSync is not implemented");
    return v8::Local<v8::Value>();
  }

  void RuntimeContext::fetchTextSourceResource(const string &url,
                                               const StringResponseCallback &responseCallback,
                                               const optional<ErrorCallback> errorCallback)
  {
    fetchResource(url, "string", [responseCallback](const v8::FunctionCallbackInfo<v8::Value> &info)
                  {
        auto isolate = info.GetIsolate();
        v8::HandleScope handleScope(isolate);
        {
          auto value = info[0].As<v8::String>();
          v8::String::Utf8Value value_utf8(isolate, value);
          auto result = string(*value_utf8, value_utf8.length());
          responseCallback(result);
        } });
  }

  string RuntimeContext::fetchTextSourceResourceSync(const string &url, bool ignoreErrors)
  {
    try
    {
      auto result = fetchResourceSync(url, "string");
      v8::String::Utf8Value result_utf8(isolate, result.As<v8::String>());
      return string(*result_utf8, result_utf8.length());
    }
    catch (const exception &e)
    {
      if (!ignoreErrors)
        throw e;
      else
        return "";
    }
  }

  void RuntimeContext::fetchArrayBufferLikeResource(const string &url,
                                                    const BufferResponseCallback &responseCallback,
                                                    const optional<ErrorCallback> errorCallback)
  {
    auto onResponse = [responseCallback](const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      auto isolate = info.GetIsolate();
      auto context = isolate->GetCurrentContext();
      v8::HandleScope handleScope(isolate);
      {
        v8::Local<v8::Value> value = info[0];
        if (!value->IsArrayBuffer() && value->IsObject())
        {
          /**
           * Node.js Buffer and ArrayBufferView objects have a `buffer` property that points to the underlying ArrayBuffer.
           */
          auto valueObject = value->ToObject(context).ToLocalChecked();
          auto bufferKey = v8::String::NewFromUtf8(isolate, "buffer").ToLocalChecked();
          value = valueObject->Get(context, bufferKey).ToLocalChecked();
        }

        /**
         * TODO: Handle this failure case?
         */
        assert(value->IsArrayBuffer());
        auto arrayBuffer = value.As<v8::ArrayBuffer>();
        responseCallback(arrayBuffer->Data(), arrayBuffer->ByteLength());
      }
    };
    auto onError = [errorCallback](const v8::FunctionCallbackInfo<v8::Value> &info)
    {
      auto isolate = info.GetIsolate();
      auto context = isolate->GetCurrentContext();
      v8::HandleScope handleScope(isolate);
      {
        v8::Local<v8::Value> error = info[0];
        v8::String::Utf8Value error_utf8(isolate, error);
        string errorStr(*error_utf8, error_utf8.length());

        if (errorCallback.has_value())
          errorCallback.value()(errorStr);
        else
          cerr << "Uncaught Fetch Error: " << errorStr << endl;
      }
    };
    fetchResource(url, "arraybuffer", onResponse, onError);
  }

  v8::Local<v8::Value>
  RuntimeContext::createWHATWGFetchImpl(v8::Local<v8::Context> context)
  {
    assert(!baseURI.empty() && baseURI != "");

    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::EscapableHandleScope handleScope(isolate);

    if (resourceLoaderValue.IsEmpty())
      throw runtime_error("ResourceLoader not set");

    auto keyString = v8::String::NewFromUtf8(isolate, "createWHATWGFetchImpl").ToLocalChecked();
    v8::Local<v8::Object> resourceLoaderObject = v8::Local<v8::Object>::New(isolate, resourceLoaderValue);
    v8::Local<v8::Function> createFetchFunction = resourceLoaderObject->Get(context, keyString).ToLocalChecked().As<v8::Function>();

    auto baseURIValue = v8::String::NewFromUtf8(isolate, baseURI.c_str()).ToLocalChecked();
    v8::Local<v8::Value> args[] = {baseURIValue};

    v8::Local<v8::Value> creatingFetchResult;
    if (!createFetchFunction->Call(context, resourceLoaderObject, 1, args).ToLocal(&creatingFetchResult) || !creatingFetchResult->IsFunction())
      throw runtime_error("createWHATWGFetchImpl() must return a new function.");
    return handleScope.Escape(creatingFetchResult);
  }

  v8::Local<v8::Value> RuntimeContext::fetchResourceInternal(const string &url, const string &responseType)
  {
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::EscapableHandleScope handleScope(isolate);

    if (resourceLoaderValue.IsEmpty())
      throw runtime_error("ResourceLoader not set");

    auto fetchKeyString = v8::String::NewFromUtf8(isolate, "fetch").ToLocalChecked();
    v8::Local<v8::Object> resourceLoaderObject = v8::Local<v8::Object>::New(isolate, resourceLoaderValue);
    v8::Local<v8::Function> fetchFunction = resourceLoaderObject->Get(context, fetchKeyString).ToLocalChecked().As<v8::Function>();

    auto urlValue = v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
    auto optionsValue = v8::Object::New(isolate);
    auto returnsAsString = v8::String::NewFromUtf8(isolate, responseType.c_str()).ToLocalChecked();
    v8::Local<v8::Value> args[] = {urlValue, optionsValue, returnsAsString};

    v8::Local<v8::Value> fetchResult;
    if (!fetchFunction->Call(context, resourceLoaderObject, 3, args).ToLocal(&fetchResult) || !fetchResult->IsPromise())
    {
      auto message = v8::String::NewFromUtf8(isolate, "Fetch function must return a promise").ToLocalChecked();
      isolate->ThrowException(v8::Exception::TypeError(message));
      return v8::Local<v8::Value>();
    }
    else
    {
      return handleScope.Escape(fetchResult);
    }
  }
}
