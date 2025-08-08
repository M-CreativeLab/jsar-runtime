#include <node/v8.h>
#include <chrono>
#include <thread>
#include "./runtime_context.hpp"
#include "../per_process.hpp"

namespace dom
{
  using namespace std;

  RuntimeContext::RuntimeContext()
      : isolate(v8::Isolate::GetCurrent())
  {
  }

  RuntimeContext::~RuntimeContext()
  {
    if (fetch_async_handle_ != nullptr)
    {
      uv_close(reinterpret_cast<uv_handle_t *>(fetch_async_handle_), [](uv_handle_t *handle)
               { delete handle; });
      fetch_async_handle_ = nullptr;
    }
    pending_fetch_requests_.clear();
  }

  void RuntimeContext::initialize()
  {
    scriptingContext = make_shared<DOMScriptingContext>(getSharedPtr());
    scripting_thread_ = this_thread::get_id();
    {
      fetch_async_handle_ = new uv_async_t;
      fetch_async_handle_->data = this;

      auto handle_fetch_requests = [](uv_async_t *handle)
      {
        auto runtime_context = static_cast<RuntimeContext *>(handle->data);
        assert(runtime_context != nullptr && "RuntimeContext async handle data is not set correctly.");
        runtime_context->execFetchRequests();
      };
      uv_async_init(TrClientContextPerProcess::Get()->getScriptingEventLoop(),
                    fetch_async_handle_,
                    handle_fetch_requests);
    }
  }

  bool RuntimeContext::inScriptingThread() const
  {
    if (!scripting_thread_.has_value())
      return false; // Not initialized yet.
    return this_thread::get_id() == scripting_thread_.value();
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
    if (inScriptingThread())
    {
      fetchResourceImpl(url, responseType, responseCallback, errorCallback);
      return;
    }
    else
    {
      unique_lock<shared_mutex> lock(pending_fetch_requests_mutex_);
      pending_fetch_requests_.emplace_back(url,
                                           responseType,
                                           std::move(responseCallback),
                                           errorCallback);
      uv_async_send(fetch_async_handle_);
    }
  }

  v8::Local<v8::Value> RuntimeContext::fetchResourceSync(const string &url, const string &responseType)
  {
    assert(inScriptingThread() &&
           "fetchResourceSync() must be called in the scripting thread");

    v8::Isolate::Scope isolateScope(isolate);
    v8::EscapableHandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    // Call the fetch function to get a promise
    v8::Local<v8::Value> promiseValue = callFetchFunction(url, responseType);
    if (!promiseValue->IsPromise())
    {
      auto message = v8::String::NewFromUtf8(isolate, "Fetch function must return a promise").ToLocalChecked();
      isolate->ThrowException(v8::Exception::TypeError(message));
      return v8::Local<v8::Value>();
    }

    v8::Local<v8::Promise> fetchPromise = promiseValue.As<v8::Promise>();
    
    // If promise is already resolved, return immediately
    if (fetchPromise->State() != v8::Promise::kPending)
    {
      if (fetchPromise->State() == v8::Promise::kRejected)
      {
        v8::Local<v8::Value> rejectValue = fetchPromise->Result();
        isolate->ThrowException(rejectValue);
        return v8::Local<v8::Value>();
      }
      return handleScope.Escape(fetchPromise->Result());
    }

    // For pending promises, we cannot safely wait in the scripting thread
    // because running the event loop recursively can cause deadlocks.
    // Instead, try processing microtasks a few times and then give up.
    int maxAttempts = 100; // Limited attempts to avoid hanging
    int attempts = 0;
    
    while (fetchPromise->State() == v8::Promise::kPending && attempts < maxAttempts)
    {
      // Process microtasks to allow immediate promise resolution
      isolate->PerformMicrotaskCheckpoint();
      attempts++;
    }

    // If still pending after microtask processing, the fetch requires I/O
    // which cannot be done synchronously from the scripting thread
    if (fetchPromise->State() == v8::Promise::kPending)
    {
      auto message = v8::String::NewFromUtf8(isolate, 
        "Cannot perform synchronous fetch from scripting thread - operation requires I/O").ToLocalChecked();
      isolate->ThrowException(v8::Exception::Error(message));
      return v8::Local<v8::Value>();
    }

    if (fetchPromise->State() == v8::Promise::kRejected)
    {
      v8::Local<v8::Value> rejectValue = fetchPromise->Result();
      isolate->ThrowException(rejectValue);
      return v8::Local<v8::Value>();
    }

    // Promise was fulfilled, return the result
    return handleScope.Escape(fetchPromise->Result());
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
    auto onResponse = [url, responseCallback](const v8::FunctionCallbackInfo<v8::Value> &info)
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

  void RuntimeContext::fetchResourceImpl(const string &url,
                                         const string &responseType,
                                         const FunctionCallback &responseCallback,
                                         const optional<FunctionCallback> errorCallback)
  {
    assert(inScriptingThread() &&
           "fetchResourceImpl() must be called in the scripting thread");

    v8::Isolate::Scope isolateScope(isolate);
    v8::HandleScope handleScope(isolate);
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::Context::Scope contextScope(context);

    v8::Local<v8::Value> promiseValue = callFetchFunction(url, responseType);
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

  void RuntimeContext::execFetchRequests()
  {
    unique_lock<shared_mutex> lock(pending_fetch_requests_mutex_);
    while (!pending_fetch_requests_.empty())
    {
      FetchRequest &request = pending_fetch_requests_.front();
      // If the URL is empty, skip the request.
      if (!request.url.empty())
      {
        fetchResourceImpl(request.url,
                          request.responseType,
                          request.success,
                          request.error);
      }
      pending_fetch_requests_.pop_front();
    }
  }

  v8::Local<v8::Value> RuntimeContext::callFetchFunction(const string &url, const string &responseType)
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
