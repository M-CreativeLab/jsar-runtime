#include <node/v8.h>
#include <crates/bindings.hpp>
#include <client/dom/runtime_context.hpp>
#include <client/fileapi/blob.hpp>
#include <client/fetch/response.hpp>
#include <client/url/url.hpp>
#include <client/script_bindings/fetch/response.hpp>

namespace endor
{
  namespace dom
  {
    using namespace std;
    using namespace v8;

    RuntimeContext::RuntimeContext()
        : isolate(Isolate::GetCurrent())
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
      if (!newBaseURI.empty())
        baseURI = newBaseURI;
      else
        baseURI = "about:blank"; // Default base URI if not set.
    }

    void FunctionCallbackWrapper(const FunctionCallbackInfo<Value> &info)
    {
      auto isolate = info.GetIsolate();
      auto context = isolate->GetCurrentContext();
      Context::Scope contextScope(context);
      HandleScope handleScope(isolate);
      {
        auto callback = reinterpret_cast<FunctionCallback *>(info.Data().As<External>()->Value());
        (*callback)(info);
        delete callback;
      }
    }

    void RuntimeContext::setResourceLoaderValue(Local<Value> value)
    {
      auto context = isolate->GetCurrentContext();
      Isolate::Scope isolateScope(isolate);
      Context::Scope contextScope(context);
      {
        resourceLoaderValue.Reset(isolate, value.As<Object>());
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

    Local<Value> RuntimeContext::fetchResourceSync(const string &url, const string &responseType)
    {
      assert(false && "fetchResourceSync is not implemented");
      return Local<Value>();
    }

    void RuntimeContext::fetchTextSourceResource(const string &url,
                                                 const StringResponseCallback &responseCallback,
                                                 const optional<ErrorCallback> errorCallback)
    {
      fetchResource(url, "string", [responseCallback](const FunctionCallbackInfo<Value> &info)
                    {
        auto isolate = info.GetIsolate();
        HandleScope handleScope(isolate);
        {
          auto value = info[0].As<String>();
          String::Utf8Value value_utf8(isolate, value);
          auto result = string(*value_utf8, value_utf8.length());
          responseCallback(result);
        } });
    }

    string RuntimeContext::fetchTextSourceResourceSync(const string &url, bool ignoreErrors)
    {
      try
      {
        auto result = fetchResourceSync(url, "string");
        String::Utf8Value result_utf8(isolate, result.As<String>());
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
      auto onResponse = [url, responseCallback](const FunctionCallbackInfo<Value> &info)
      {
        auto isolate = info.GetIsolate();
        auto context = isolate->GetCurrentContext();
        HandleScope handleScope(isolate);
        {
          Local<Value> value = info[0];
          if (!value->IsArrayBuffer() && value->IsObject())
          {
            /**
           * Node.js Buffer and ArrayBufferView objects have a `buffer` property that points to the underlying ArrayBuffer.
           */
            auto valueObject = value->ToObject(context).ToLocalChecked();
            auto bufferKey = String::NewFromUtf8(isolate, "buffer").ToLocalChecked();
            value = valueObject->Get(context, bufferKey).ToLocalChecked();
          }

          /**
         * TODO: Handle this failure case?
         */
          assert(value->IsArrayBuffer());
          auto arrayBuffer = value.As<ArrayBuffer>();
          responseCallback(arrayBuffer->Data(), arrayBuffer->ByteLength());
        }
      };
      auto onError = [errorCallback](const FunctionCallbackInfo<Value> &info)
      {
        auto isolate = info.GetIsolate();
        auto context = isolate->GetCurrentContext();
        HandleScope handleScope(isolate);
        {
          Local<Value> error = info[0];
          String::Utf8Value error_utf8(isolate, error);
          string errorStr(*error_utf8, error_utf8.length());

          if (errorCallback.has_value())
            errorCallback.value()(errorStr);
          else
            cerr << "Uncaught Fetch Error: " << errorStr << endl;
        }
      };
      fetchResource(url, "arraybuffer", onResponse, onError);
    }

    Local<Value> RuntimeContext::createWHATWGFetchImpl(Local<Context> context)
    {
      assert(!baseURI.empty() && baseURI != "");

      Isolate::Scope isolateScope(isolate);
      Context::Scope contextScope(context);
      EscapableHandleScope handleScope(isolate);

      if (resourceLoaderValue.IsEmpty())
        throw runtime_error("ResourceLoader not set");

      auto keyString = String::NewFromUtf8(isolate, "createWHATWGFetchImpl").ToLocalChecked();
      Local<Object> resourceLoaderObject = Local<Object>::New(isolate, resourceLoaderValue);
      Local<Function> createFetchFunction = resourceLoaderObject->Get(context, keyString).ToLocalChecked().As<Function>();

      auto baseURIValue = String::NewFromUtf8(isolate, baseURI.c_str()).ToLocalChecked();
      Local<Value> args[] = {baseURIValue};

      Local<Value> creatingFetchResult;
      if (!createFetchFunction->Call(context, resourceLoaderObject, 1, args).ToLocal(&creatingFetchResult) || !creatingFetchResult->IsFunction())
        throw runtime_error("createWHATWGFetchImpl() must return a new function.");
      return handleScope.Escape(creatingFetchResult);
    }

    Local<Promise> RuntimeContext::sendWHATWGFetchRequest(Isolate *isolate,
                                                          Local<Value> resource,
                                                          Local<Value> options)
    {
      assert(!baseURI.empty() && baseURI != "");

      EscapableHandleScope scope(isolate);
      Local<Context> context = isolate->GetCurrentContext();

      if (resourceLoaderValue.IsEmpty()) [[unlikely]]
      {
        throw runtime_error("ResourceLoader not set");
      }

      if (resource->IsString())
      {
        // Check if the string starts with "blob:"
        String::Utf8Value utf8Value(isolate, resource);
        string resourceStr = string(*utf8Value);
        if (resourceStr.rfind("blob:", 0) == 0)
        {
          // It's a blob URL, resolve it to a Blob object
          Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
          shared_ptr<client_fileapi::Blob> blob = client_url::URL::BlobRegistry.get(resourceStr);
          resolver->Resolve(context, script_bindings::Response::NewInstance(isolate, blob)).ToChecked();
          return scope.Escape(resolver->GetPromise());
        }
      }

      Local<String> methodName = String::NewFromUtf8(isolate, "sendWHATWGFetchRequest").ToLocalChecked();
      Local<Object> resourceLoaderObject = Local<Object>::New(isolate, resourceLoaderValue);
      Local<Function> sendFetchRequestFunc = resourceLoaderObject->Get(context,
                                                                       methodName)
                                               .ToLocalChecked()
                                               .As<Function>();

      auto baseURIValue = String::NewFromUtf8(isolate, baseURI.c_str()).ToLocalChecked();
      Local<Value> args[] = {baseURIValue, resource, options};

      TryCatch tryCatch(isolate);
      Local<Value> responsePromise;
      if (!sendFetchRequestFunc->Call(context, resourceLoaderObject, 3, args).ToLocal(&responsePromise) ||
          tryCatch.HasCaught())
      {
        Local<Promise::Resolver> resolver = Promise::Resolver::New(context).ToLocalChecked();
        resolver->Reject(context, tryCatch.Exception()).ToChecked();
        return scope.Escape(resolver->GetPromise());
      }
      else
      {
        assert(responsePromise->IsPromise() &&
               "sendWHATWGFetchRequest() must return a Promise on success.");
        return scope.Escape(responsePromise.As<Promise>());
      }
    }

    // Normalize URL by combining with baseURI if relative
    static string normalize(const string &url, const string &baseURI)
    {
      if (url.find("://") != string::npos || url.rfind("about:", 0) == 0)
      {
        // url is absolute
        return url;
      }
      else
      {
        // url is relative, combine with baseURI
        if (!baseURI.empty() && baseURI != "about:blank")
        {
          return crates::UrlHelper::CreateUrlStringWithPath(baseURI, url);
        }
        else
        {
          return url;
        }
      }
    }

    void RuntimeContext::fetchResourceImpl(const string &url,
                                           const string &responseType,
                                           const FunctionCallback &responseCallback,
                                           const optional<FunctionCallback> errorCallback)
    {
      assert(inScriptingThread() &&
             "fetchResourceImpl() must be called in the scripting thread");

      Isolate::Scope isolateScope(isolate);
      HandleScope handleScope(isolate);
      Local<Context> context = isolate->GetCurrentContext();
      Context::Scope contextScope(context);

      Local<External> resolveCallbackExternal = External::New(isolate, new FunctionCallback(responseCallback));
      Local<Function> resolve = Function::New(context,
                                              FunctionCallbackWrapper,
                                              resolveCallbackExternal)
                                  .ToLocalChecked();
      Local<Function> reject;
      if (errorCallback.has_value())
      {
        Local<External> rejectCallbackExternal = External::New(isolate, new FunctionCallback(errorCallback.value()));
        reject = Function::New(context, FunctionCallbackWrapper, rejectCallbackExternal).ToLocalChecked();
      }

      // If the URL is a blob URL, handle it directly
      if (url.rfind("blob:", 0) == 0)
      {
        // It's a blob URL, resolve it to a Blob object
        shared_ptr<client_fileapi::Blob> blob = client_url::URL::BlobRegistry.get(url);
        if (responseType == "string")
        {
          string text = blob->text().get_future().get();
          Local<String> result = String::NewFromUtf8(isolate, text.c_str()).ToLocalChecked();
          Local<Value> args[] = {result};
          resolve->Call(context, Undefined(isolate), 1, args).ToLocalChecked();
        }
        else if (responseType == "json")
        {
          string text = blob->text().get_future().get();
          Local<String> jsonString = String::NewFromUtf8(isolate, text.c_str()).ToLocalChecked();
          Local<Value> jsonValue;
          if (!JSON::Parse(context, jsonString).ToLocal(&jsonValue))
          {
            auto msg = "Failed to parse JSON from blob URL: " + url;
            if (errorCallback.has_value())
            {
              Local<Value> args[] = {Exception::TypeError(String::NewFromUtf8(isolate, msg.c_str()).ToLocalChecked())};
              reject->Call(context, Undefined(isolate), 1, args).ToLocalChecked();
            }
            else
            {
              cerr << "Uncaught Fetch Error: Failed to parse JSON from blob URL" << endl;
            }
          }
          else
          {
            Local<Value> args[] = {jsonValue};
            resolve->Call(context, Undefined(isolate), 1, args).ToLocalChecked();
          }
        }
        else if (responseType == "arraybuffer")
        {
          vector<uint8_t> data = blob->bytes().get_future().get();
          Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, data.size());
          memcpy(arrayBuffer->GetBackingStore()->Data(), data.data(), data.size());
          Local<Value> args[] = {arrayBuffer};
          resolve->Call(context, Undefined(isolate), 1, args).ToLocalChecked();
        }
        else
        {
          assert(false && "Unsupported responseType for blob URL");
        }
        return;
      }

      // Call to the `fetch` function
      Local<Value> promiseValue = callFetchFunction(normalize(url, baseURI), responseType);
      if (!promiseValue->IsPromise())
        return;

      Local<Promise> fetchPromise = promiseValue.As<Promise>();

      // Schedule the callbacks
      if (errorCallback.has_value())
      {
        fetchPromise->Then(context, resolve, reject).ToLocalChecked();
      }
      else
      {
        fetchPromise->Then(context, resolve).ToLocalChecked();
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

    Local<Value> RuntimeContext::callFetchFunction(const string &url, const string &responseType)
    {
      auto context = isolate->GetCurrentContext();
      Isolate::Scope isolateScope(isolate);
      Context::Scope contextScope(context);
      EscapableHandleScope handleScope(isolate);

      if (resourceLoaderValue.IsEmpty())
        throw runtime_error("ResourceLoader not set");

      auto fetchKeyString = String::NewFromUtf8(isolate, "fetch").ToLocalChecked();
      Local<Object> resourceLoaderObject = Local<Object>::New(isolate, resourceLoaderValue);
      Local<Function> fetchFunction = resourceLoaderObject->Get(context, fetchKeyString).ToLocalChecked().As<Function>();

      auto urlValue = String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
      auto optionsValue = Object::New(isolate);
      auto returnsAsString = String::NewFromUtf8(isolate, responseType.c_str()).ToLocalChecked();
      Local<Value> args[] = {urlValue, optionsValue, returnsAsString};

      Local<Value> fetchResult;
      if (!fetchFunction->Call(context, resourceLoaderObject, 3, args).ToLocal(&fetchResult) || !fetchResult->IsPromise())
      {
        auto message = String::NewFromUtf8(isolate, "Fetch function must return a promise").ToLocalChecked();
        isolate->ThrowException(Exception::TypeError(message));
        return Local<Value>();
      }
      else
      {
        return handleScope.Escape(fetchResult);
      }
    }
  }
} // namespace endor
