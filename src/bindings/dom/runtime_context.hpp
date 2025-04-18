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
      };
    }
    RuntimeContextBase(const Napi::CallbackInfo &info) : Napi::ObjectWrap<ObjectType>(info),
                                                         contextImpl(std::make_shared<ContextType>())
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

  protected:
    shared_ptr<ContextType> contextImpl;
  };
}
