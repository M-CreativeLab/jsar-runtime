#include "./window.hpp"
#include "./location.hpp"

namespace browserbinding
{
  thread_local Napi::FunctionReference *Window::constructor;
  void Window::Init(Napi::Env env)
  {
    auto props = dombinding::EventTargetWrap<Window, browser::Window>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<Window>>(
        {InstanceMethod("alert", &Window::Alert, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("blur", &Window::Blur, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("close", &Window::Close, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("confirm", &Window::Confirm, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("focus", &Window::Focus, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("open", &Window::Open, napi_property_attributes::napi_default_jsproperty),
         InstanceMethod("prompt", &Window::Prompt, napi_property_attributes::napi_default_jsproperty)});
    props.insert(props.end(), added.begin(), added.end());

    Napi::Function func = DefineClass(env, "Window", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Window", func);
  }

  Napi::Object Window::NewInstance(Napi::Env env, std::string url)
  {
    Napi::EscapableHandleScope scope(env);
    auto windowObject = constructor->New({});
    auto locationObject = Location::NewInstance(env, url);
    windowObject.Set("location", locationObject);
    windowObject.Set("origin", locationObject.Get("origin"));
    return scope.Escape(windowObject).ToObject();
  }

  Window::Window(const Napi::CallbackInfo &info)
      : dombinding::EventTargetWrap<Window, browser::Window>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    setEventTarget(make_shared<browser::Window>());
  }

  Napi::Value Window::Alert(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::string message = "";
    if (info.Length() >= 1 && info[0].IsString())
      message = info[0].As<Napi::String>().Utf8Value();

    eventTarget->alert(message);
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

    eventTarget->close();
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

    string url = "about:blank";
    if (info.Length() >= 1 && info[0].IsString())
      url = info[0].As<Napi::String>().Utf8Value();

    browser::WindowTarget target = browser::WindowTarget::Self;
    if (info.Length() >= 2 && info[1].IsString())
    {
      string targetStr = info[1].As<Napi::String>().Utf8Value();
      if (targetStr == "_blank")
        target = browser::WindowTarget::Blank;
      else if (targetStr == "_blankClassic")
        target = browser::WindowTarget::BlankClassic;
      else if (targetStr == "_parent")
        target = browser::WindowTarget::Parent;
      else if (targetStr == "_top")
        target = browser::WindowTarget::Top;
    }

    if (target != browser::WindowTarget::BlankClassic)
    {
      Napi::TypeError::New(env, "Only \"_blankClassic\" is supported").ThrowAsJavaScriptException();
      return env.Undefined();
    }

    // TODO: support parsing window features
    eventTarget->open(url, target);
    return env.Undefined();
  }

  Napi::Value Window::Prompt(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    std::string message = "";
    if (info.Length() >= 1 && info[0].IsString())
      message = info[0].As<Napi::String>().Utf8Value();

    std::string defaultValue = "";
    if (info.Length() >= 2 && info[1].IsString())
      defaultValue = info[1].As<Napi::String>().Utf8Value();

    eventTarget->prompt(message, defaultValue);
    return env.Undefined();
  }
}
