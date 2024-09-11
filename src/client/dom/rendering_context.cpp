#include <node/v8.h>
#include "./rendering_context.hpp"

namespace dom
{
  void DocumentRenderingContext::setResourceLoaderValue(v8::Local<v8::Value> value)
  {
    auto context = isolate->GetCurrentContext();
    v8::Isolate::Scope isolateScope(isolate);
    v8::Context::Scope contextScope(context);
    {
      resourceLoaderValue.Reset(isolate, value.As<v8::Object>());
    }
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

  void DocumentRenderingContext::fetchResource(const std::string &url, const std::string &responseType, const ResponseCallback &callback)
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

  void DocumentRenderingContext::fetchTextSourceResource(const std::string &url, const StringResponseCallback &callback)
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

  shared_ptr<DOMScript> DocumentRenderingContext::createScript(const std::string &url, SourceTextType type)
  {
    return scriptingContext->create(shared_from_this(), url, type);
  }
}
