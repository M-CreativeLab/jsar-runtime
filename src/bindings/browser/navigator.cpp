#include "navigator.hpp"

namespace browserbinding
{
  thread_local Napi::FunctionReference *Navigator::constructor;

  void Navigator::Init(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);

    auto props = std::vector<Napi::ClassPropertyDescriptor<Navigator>>(
      {InstanceAccessor("onLine", &Navigator::OnLineGetter, nullptr, napi_property_attributes::napi_enumerable)});

    Napi::Function func = DefineClass(env, "Navigator", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Navigator", func);
  }

  Napi::Object Navigator::NewInstance(Napi::Env env, std::shared_ptr<browser::Window> window)
  {
    Napi::EscapableHandleScope scope(env);
    auto navigatorObject = constructor->New({});

    // Store reference to window
    auto navigator = Napi::ObjectWrap<Navigator>::Unwrap(navigatorObject);
    navigator->window_ = window;

    return scope.Escape(navigatorObject).ToObject();
  }

  Navigator::Navigator(const Napi::CallbackInfo &info)
      : Napi::ObjectWrap<Navigator>(info)
  {
  }

  Napi::Value Navigator::OnLineGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto window = window_.lock();
    if (window)
    {
      bool isOnline = window->navigator_onLine();
      return Napi::Boolean::New(env, isOnline);
    }

    // Default to offline if window is gone
    return Napi::Boolean::New(env, false);
  }
} // namespace browserbinding