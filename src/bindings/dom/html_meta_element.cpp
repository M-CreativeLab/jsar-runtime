#include <assert.h>
#include "./html_meta_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *HTMLMetaElement::constructor;

  vector<Napi::ClassPropertyDescriptor<HTMLMetaElement>> HTMLMetaElement::GetClassProperties()
  {
    using T = HTMLMetaElement;
    auto props = HTMLElementBase<HTMLMetaElement, dom::HTMLMetaElement>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLMetaElement>>(
        {
            T::InstanceAccessor("content", &T::ContentGetter, &T::ContentSetter),
            T::InstanceAccessor("httpEquiv", &T::HttpEquivGetter, &T::HttpEquivSetter),
            T::InstanceAccessor("media", &T::MediaGetter, &T::MediaSetter),
            T::InstanceAccessor("name", &T::NameGetter, &T::NameSetter),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLMetaElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLMetaElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLMetaElement", func);
  }

  Napi::Value HTMLMetaElement::ContentGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), this->node->getContent());
  }

  void HTMLMetaElement::ContentSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setContent(value.ToString().Utf8Value());
  }

  Napi::Value HTMLMetaElement::HttpEquivGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), this->node->getHttpEquiv());
  }

  void HTMLMetaElement::HttpEquivSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setHttpEquiv(value.ToString().Utf8Value());
  }

  Napi::Value HTMLMetaElement::MediaGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), this->node->getMedia());
  }

  void HTMLMetaElement::MediaSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setMedia(value.ToString().Utf8Value());
  }

  Napi::Value HTMLMetaElement::NameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), this->node->getName());
  }

  void HTMLMetaElement::NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    this->node->setName(value.ToString().Utf8Value());
  }
}
