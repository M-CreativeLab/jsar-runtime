#pragma once

#include <napi.h>
#include "idgen.hpp"
#include "common/utility.hpp"
#include "client/graphics/webgl_object.hpp"

namespace webgl
{

#define WEBGL_OBJECT_KEY 0xff32

  template <typename ObjectType, typename HandleType = client_graphics::WebGLObject>
  class WebGLObjectBase : public Napi::ObjectWrap<ObjectType>
  {
  protected:
    /**
     * Create a new instance of the class inherited from `WebGLObjectBase`.
     * 
     * @param env The environment that the instance will be created in.
     * @param handle The native handle of the instance.
     * @returns The new instance of the class inherited from `WebGLObjectBase`.
     */
    static Napi::Object NewInstance(Napi::Env env, std::shared_ptr<HandleType> handle)
    {
      Napi::EscapableHandleScope scope(env);
      SharedReference<HandleType> handleRef(handle);
      auto external = Napi::External<SharedReference<HandleType>>::New(env, &handleRef);
      auto instance = ObjectType::constructor->New({external});
      return scope.Escape(instance).ToObject();
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

      SharedReference<HandleType> handleRef = *info[0].As<Napi::External<SharedReference<HandleType>>>().Data();
      handle_ = handleRef.value;

      auto jsThis = info.This().ToObject();
      jsThis.Set("id", Napi::Number::New(env, id()));
    }

  public:
    /**
     * @returns the native handle of this `WebGLObject`.
     */
    std::shared_ptr<HandleType> handle()
    {
      return handle_;
    }
    /**
     * @returns the id of this `WebGLObject` or zero if the handle is null.
     */
    int id()
    {
      return handle_ == nullptr ? 0 : handle_->id;
    }

  protected:
    std::shared_ptr<HandleType> handle_;
  };

  class WebGLObject : public WebGLObjectBase<WebGLObject>
  {
  public:
    static void Init(Napi::Env env);
    WebGLObject(const Napi::CallbackInfo &info) : WebGLObjectBase<WebGLObject>(info) {}

  private:
    static thread_local Napi::FunctionReference *constructor;
  };
}
