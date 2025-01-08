#include "./css_style_declaration.hpp"

namespace cssombinding
{
  using namespace std;

  void CSSStyleDeclaration::Init(Napi::Env env)
  {
#define MODULE_NAME "CSSStyleDeclaration"
    auto props = {
        InstanceAccessor("cssText", &CSSStyleDeclaration::CssTextGetter, &CSSStyleDeclaration::CssTextSetter),
        InstanceAccessor("length", &CSSStyleDeclaration::LengthGetter, nullptr),
        InstanceMethod("getPropertyPriority", &CSSStyleDeclaration::GetPropertyPriority),
        InstanceMethod("getPropertyValue", &CSSStyleDeclaration::GetPropertyValue),
        InstanceMethod("item", &CSSStyleDeclaration::Item),
        InstanceMethod("removeProperty", &CSSStyleDeclaration::RemoveProperty),
        InstanceMethod("setProperty", &CSSStyleDeclaration::SetProperty),
        InstanceMethod("toString", &CSSStyleDeclaration::ToString),
    };
    Napi::Function func = DefineClass(env, MODULE_NAME, props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set(MODULE_NAME, func);
#undef MODULE_NAME
  }

  Napi::Object CSSStyleDeclaration::NewInstance(Napi::Env env, shared_ptr<client_cssom::CSSStyleDeclaration> handle)
  {
    Napi::EscapableHandleScope scope(env);
    SharedReference<client_cssom::CSSStyleDeclaration> sharedHandle(handle);
    auto external = Napi::External<SharedReference<client_cssom::CSSStyleDeclaration>>::New(env, &sharedHandle);
    Napi::Object obj = constructor->New({external});
    return scope.Escape(obj).ToObject();
  }

  CSSStyleDeclaration::CSSStyleDeclaration(const Napi::CallbackInfo &info) : Napi::ObjectWrap<CSSStyleDeclaration>(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsExternal())
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }

    auto external = info[0].As<Napi::External<SharedReference<client_cssom::CSSStyleDeclaration>>>();
    auto handleRef = external.Data();
    if (handleRef == nullptr)
    {
      Napi::TypeError::New(env, "Illegal constructor").ThrowAsJavaScriptException();
      return;
    }
    handle_ = handleRef->value;
  }

  Napi::Value CSSStyleDeclaration::CssTextGetter(const Napi::CallbackInfo &info)
  {
    auto cssText = useHandle<string>([](client_cssom::CSSStyleDeclaration &handle)
                                     { return handle.cssText(); }, "");
    return Napi::String::New(info.Env(), cssText);
  }

  void CSSStyleDeclaration::CssTextSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    // TODO: Implement setter
  }

  Napi::Value CSSStyleDeclaration::LengthGetter(const Napi::CallbackInfo &info)
  {
    auto length = useHandle<size_t>([](client_cssom::CSSStyleDeclaration &handle)
                                    { return handle.length(); }, 0);
    return Napi::Number::New(info.Env(), length);
  }

  Napi::Value CSSStyleDeclaration::GetPropertyPriority(const Napi::CallbackInfo &info)
  {
    if (info.Length() != 1 || !info[0].IsString())
    {
      Napi::TypeError::New(info.Env(), "Invalid arguments").ThrowAsJavaScriptException();
      return info.Env().Undefined();
    }

    auto propertyName = info[0].As<Napi::String>().Utf8Value();
    auto priority = useHandle<client_cssom::CSSPropertyPriority>([&propertyName](client_cssom::CSSStyleDeclaration &handle)
                                                                 { return handle.getPropertyPriority(propertyName); }, client_cssom::CSSPropertyPriority::Normal);
    return Napi::Number::New(info.Env(), static_cast<int>(priority));
  }

  Napi::Value CSSStyleDeclaration::GetPropertyValue(const Napi::CallbackInfo &info)
  {
    if (info.Length() != 1 || !info[0].IsString())
    {
      Napi::TypeError::New(info.Env(), "Invalid arguments").ThrowAsJavaScriptException();
      return info.Env().Undefined();
    }

    auto propertyName = info[0].As<Napi::String>().Utf8Value();
    auto value = useHandle<string>([&propertyName](client_cssom::CSSStyleDeclaration &handle)
                                   { return handle.getPropertyValue(propertyName); }, "");
    return Napi::String::New(info.Env(), value);
  }

  Napi::Value CSSStyleDeclaration::Item(const Napi::CallbackInfo &info)
  {
    if (info.Length() != 1 || !info[0].IsNumber())
    {
      Napi::TypeError::New(info.Env(), "Invalid arguments").ThrowAsJavaScriptException();
      return info.Env().Undefined();
    }

    // TODO: Implement item().
    return info.Env().Undefined();
  }

  void CSSStyleDeclaration::RemoveProperty(const Napi::CallbackInfo &info)
  {
    if (info.Length() != 1 || !info[0].IsString())
    {
      Napi::TypeError::New(info.Env(), "Invalid arguments").ThrowAsJavaScriptException();
      return;
    }

    auto propertyName = info[0].As<Napi::String>().Utf8Value();
    useHandle([&propertyName](client_cssom::CSSStyleDeclaration &handle)
              { handle.removeProperty(propertyName); });
  }

  void CSSStyleDeclaration::SetProperty(const Napi::CallbackInfo &info)
  {
    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString())
    {
      Napi::TypeError::New(info.Env(), "Invalid arguments").ThrowAsJavaScriptException();
      return;
    }

    auto propertyName = info[0].As<Napi::String>().Utf8Value();
    auto value = info[1].As<Napi::String>().Utf8Value();
    useHandle([&propertyName, &value](client_cssom::CSSStyleDeclaration &handle)
              { handle.setProperty(propertyName, value); });
  }

  Napi::Value CSSStyleDeclaration::ToString(const Napi::CallbackInfo &info)
  {
    auto str = useHandle<string>([](client_cssom::CSSStyleDeclaration &handle)
                                 { return handle.cssText(); }, "");
    return Napi::String::New(info.Env(), str);
  }
}
