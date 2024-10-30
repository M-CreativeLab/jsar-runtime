#include "./window.hpp"

namespace browserbinding
{
  thread_local Napi::FunctionReference *Window::constructor;
  void Window::Init(Napi::Env env)
  {
    Napi::Function func = DefineClass(env, "Window",
                                      {
                                          InstanceMethod("alert", &Window::Alert),
                                          InstanceMethod("blur", &Window::Blur),
                                          InstanceMethod("close", &Window::Close),
                                          InstanceMethod("confirm", &Window::Confirm),
                                          InstanceMethod("focus", &Window::Focus),
                                          InstanceMethod("open", &Window::Open),
                                          InstanceMethod("prompt", &Window::Prompt),
                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Window", func);
  }

  Napi::Object Window::NewInstance(Napi::Env env, std::string url)
  {
    Napi::EscapableHandleScope scope(env);
    /**
     * TODO: Implement the following code.
     */
    auto instance = constructor->New({});
    return scope.Escape(instance).ToObject();
  }

  Window::Window(const Napi::CallbackInfo &info)
      : dombinding::EventTargetWrap<Window, browser::Window>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    // TODO: Implement the following code.
  }

  Napi::Value Window::Alert(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string message = info[0].As<Napi::String>().Utf8Value();
    return env.Undefined();
  }

  Napi::Value Window::Blur(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value Window::Close(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value Window::Confirm(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string message = info[0].As<Napi::String>().Utf8Value();
    return env.Undefined();
  }

  Napi::Value Window::Focus(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return env.Undefined();
  }

  Napi::Value Window::Open(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string url = info[0].As<Napi::String>().Utf8Value();
    // TODO
    return env.Undefined();
  }

  Napi::Value Window::Prompt(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1 || !info[0].IsString())
    {
      Napi::TypeError::New(env, "Invalid argument").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    std::string message = info[0].As<Napi::String>().Utf8Value();
    // TODO
    return env.Undefined();
  }
}
