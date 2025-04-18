#include "./html_script_element.hpp"

namespace dombinding
{
  thread_local Napi::FunctionReference *HTMLScriptElement::constructor;
  vector<Napi::ClassPropertyDescriptor<HTMLScriptElement>> HTMLScriptElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLScriptElement;
    auto props = HTMLElementBase<HTMLScriptElement, dom::HTMLScriptElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLScriptElement>>(
        {
            T::InstanceAccessor("async", &T::AsyncGetter, &T::AsyncSetter, napi_default_jsproperty),
            T::InstanceAccessor("src", &T::SrcGetter, &T::SrcSetter, napi_default_jsproperty),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLScriptElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLScriptElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLScriptElement", func);
  }

  Napi::Value HTMLScriptElement::AsyncGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Boolean::New(env, this->node->async);
  }

  void HTMLScriptElement::AsyncSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto asyncValue = value.ToBoolean().Value();
    this->node->setAsync(asyncValue);
  }

  Napi::Value HTMLScriptElement::SrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::String::New(env, this->node->src);
  }

  void HTMLScriptElement::SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto srcValue = value.ToString().Utf8Value();
    this->node->setSrc(srcValue);
  }
}
