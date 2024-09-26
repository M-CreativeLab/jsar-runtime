#include <node/v8.h>
#include "./runtime_context.hpp"

namespace dom
{
  void RuntimeContext::initialize()
  {
    scriptingContext = make_shared<DOMScriptingContext>(getSharedPtr());
  }

  void RuntimeContext::setBaseURI(const std::string newBaseURI)
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
      auto callback = reinterpret_cast<ResponseCallback *>(info.Data().As<v8::External>()->Value());
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
    }
  }

  void RuntimeContext::fetchResource(const std::string &url, const std::string &responseType, const ResponseCallback &callback)
  {
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::HandleScope handleScope(isolate);
    {
      if (resourceLoaderValue.IsEmpty())
        throw std::runtime_error("Resource loader not set");

      auto fetchKeyString = v8::String::NewFromUtf8(isolate, "fetch").ToLocalChecked();
      v8::Local<v8::Object> resourceLoaderObject = v8::Local<v8::Object>::New(isolate, resourceLoaderValue);
      v8::Local<v8::Function> fetchFunction = resourceLoaderObject->Get(context, fetchKeyString).ToLocalChecked().As<v8::Function>();
      v8::Local<v8::External> callbackExternal = v8::External::New(isolate, new ResponseCallback(callback));

      auto urlValue = v8::String::NewFromUtf8(isolate, url.c_str()).ToLocalChecked();
      auto optionsValue = v8::Object::New(isolate);
      auto returnsAsString = v8::String::NewFromUtf8(isolate, responseType.c_str()).ToLocalChecked();
      v8::Local<v8::Value> args[] = {urlValue, optionsValue, returnsAsString};

      v8::Local<v8::Promise> fetchPromise;
      v8::Local<v8::Value> fetchResult;
      if (!fetchFunction->Call(context, resourceLoaderObject, 3, args).ToLocal(&fetchResult) || !fetchResult->IsPromise())
      {
        auto message = v8::String::NewFromUtf8(isolate, "Fetch function must return a promise").ToLocalChecked();
        isolate->ThrowException(v8::Exception::TypeError(message));
        return;
      }
      else
      {
        fetchPromise = fetchResult.As<v8::Promise>();
      }

      auto resolve = v8::Function::New(context, ResolveResource, callbackExternal);
      // Schedule the callbacks
      fetchPromise->Then(context, resolve.ToLocalChecked())
          .ToLocalChecked();
    }
  }

  void RuntimeContext::fetchTextSourceResource(const std::string &url, const StringResponseCallback &callback)
  {
    fetchResource(url, "string", [callback](const v8::FunctionCallbackInfo<v8::Value> &info)
                  {
        auto isolate = info.GetIsolate();
        v8::HandleScope handleScope(isolate);
        {
          auto value = info[0].As<v8::String>();
          v8::String::Utf8Value value_utf8(isolate, value);
          auto result = string(*value_utf8, value_utf8.length());
          callback(result);
        } });
  }

  void RuntimeContext::fetchArrayBufferLikeResource(const std::string &url, const BufferResponseCallback &callback)
  {
    fetchResource(url, "arraybuffer", [callback](const v8::FunctionCallbackInfo<v8::Value> &info)
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
          callback(arrayBuffer->Data(), arrayBuffer->ByteLength());
        } });
  }

  v8::Local<v8::Value> RuntimeContext::createWHATWGFetchImpl(v8::Local<v8::Context> context)
  {
    assert(!baseURI.empty() && baseURI != "");

    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    v8::EscapableHandleScope handleScope(isolate);

    if (resourceLoaderValue.IsEmpty())
      throw std::runtime_error("Resource loader not set");

    auto keyString = v8::String::NewFromUtf8(isolate, "createWHATWGFetchImpl").ToLocalChecked();
    v8::Local<v8::Object> resourceLoaderObject = v8::Local<v8::Object>::New(isolate, resourceLoaderValue);
    v8::Local<v8::Function> createFetchFunction = resourceLoaderObject->Get(context, keyString).ToLocalChecked().As<v8::Function>();

    auto baseURIValue = v8::String::NewFromUtf8(isolate, baseURI.c_str()).ToLocalChecked();
    v8::Local<v8::Value> args[] = {baseURIValue};

    v8::Local<v8::Value> creatingFetchResult;
    if (!createFetchFunction->Call(context, resourceLoaderObject, 1, args).ToLocal(&creatingFetchResult) || !creatingFetchResult->IsFunction())
      throw std::runtime_error("createWHATWGFetchImpl() must return a new function.");
    return handleScope.Escape(creatingFetchResult);
  }
}
