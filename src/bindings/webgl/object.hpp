#pragma once

#include <napi.h>
#include "idgen.hpp"

namespace webgl
{

#define WEBGL_OBJECT_KEY 0xff32

  template <typename ObjectType>
  class WebGLObjectBase : public Napi::ObjectWrap<ObjectType>
  {
  protected:
    static Napi::Object NewInstance(Napi::Env env)
    {
      Napi::EscapableHandleScope scope(env);
      int key = WEBGL_OBJECT_KEY;
      Napi::Object obj = ObjectType::constructor->New({Napi::External<int>::New(env, &key)});
      return scope.Escape(napi_value(obj)).ToObject();
    }

  public:
    WebGLObjectBase(const Napi::CallbackInfo &info)
        : Napi::ObjectWrap<ObjectType>(info)
    {
      Napi::Env env = info.Env();
      Napi::HandleScope scope(env);

      if (info.Length() != 1 || !info[0].IsExternal())
      {
        Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
        return;
      }

      Napi::External<int> ext = info[0].As<Napi::External<int>>();
      if (*ext.Data() != WEBGL_OBJECT_KEY)
      {
        Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
        return;
      }

      static TrIdGenerator idGen(1);
      id = idGen.get();

      auto jsThis = info.This().As<Napi::Object>();
      jsThis.Set("_id", Napi::Number::New(env, id));
    }

  public:
    uint32_t id;
  };

  class WebGLObject : public WebGLObjectBase<WebGLObject>
  {
  public:
    static void Init(Napi::Env env);
    WebGLObject(const Napi::CallbackInfo &info);

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
