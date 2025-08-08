#pragma once

#include <memory>
#include <napi.h>
#include "client/dom/runtime_context.hpp"

namespace dombinding
{
  template <typename T = v8::Value>
  inline v8::Local<T> convertNapiValueToV8Local(napi_value v)
  {
    v8::Local<T> local;
    memcpy(static_cast<void *>(&local), &v, sizeof(v));
    return local;
  }

  template <typename ObjectType, typename ContextType>
  class RuntimeContextBase : public Napi::ObjectWrap<ObjectType>
  {
  public:
    static vector<Napi::ClassPropertyDescriptor<ObjectType>> GetClassProperties(Napi::Env env)
    {
      using T = RuntimeContextBase<ObjectType, ContextType>;
      return {
        T::InstanceMethod("setResourceLoader", &T::SetResourceLoader),
        T::InstanceMethod("fetchResourceSync", &T::FetchResourceSync),
      };
    }
    RuntimeContextBase(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ObjectType>(info)
        , contextImpl(std::make_shared<ContextType>())
    {
      contextImpl->initialize();
      contextImpl->scriptingContext->enableDynamicImport();
    }

  private:
    Napi::Value SetResourceLoader(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() == 0 && !info[0].IsObject())
      {
        Napi::TypeError::New(env, "Expected an object.").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      auto jsResourceLoader = info[0].As<Napi::Object>();
      if (jsResourceLoader.Has("fetch") && jsResourceLoader.Get("fetch").IsFunction())
      {
        contextImpl->setResourceLoaderValue(convertNapiValueToV8Local(jsResourceLoader));
        return env.Undefined();
      }
      else
      {
        Napi::TypeError::New(env, "Expected an object with a fetch function.").ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

    Napi::Value FetchResourceSync(const Napi::CallbackInfo &info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() < 2)
      {
        Napi::TypeError::New(env, "Expected at least 2 arguments: url and responseType").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      if (!info[0].IsString() || !info[1].IsString())
      {
        Napi::TypeError::New(env, "Expected string arguments").ThrowAsJavaScriptException();
        return env.Undefined();
      }

      std::string url = info[0].As<Napi::String>();
      std::string responseType = info[1].As<Napi::String>();

      try
      {
        v8::Local<v8::Value> result = contextImpl->fetchResourceSync(url, responseType);
        if (result.IsEmpty())
        {
          return env.Undefined();
        }

        // Convert V8 value to NAPI value
        napi_value napiResult;
        memcpy(&napiResult, static_cast<void *>(&result), sizeof(result));
        return Napi::Value(env, napiResult);
      }
      catch (const std::exception &e)
      {
        Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
        return env.Undefined();
      }
    }

  protected:
    shared_ptr<ContextType> contextImpl;
  };
}
