#include <assert.h>
#include "./html_model_element.hpp"
#include "./document-inl.hpp"

using namespace std;

namespace dombinding
{
  HTMLModelElement::HTMLModelElement(const Napi::CallbackInfo &info)
      : HTMLElementBase<HTMLModelElement, dom::HTMLModelElement>(info)
  {
  }

  vector<Napi::ClassPropertyDescriptor<HTMLModelElement>> HTMLModelElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLModelElement;
    auto props = HTMLElementBase<HTMLModelElement, dom::HTMLModelElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLModelElement>>(
      {
        T::InstanceAccessor("src", &T::SrcGetter, &T::SrcSetter),
        T::InstanceAccessor("type", &T::TypeGetter, &T::TypeSetter),
        T::InstanceAccessor("autoplay", &T::AutoplayGetter, &T::AutoplaySetter),
        T::InstanceAccessor("loading", &T::LoadingGetter, &T::LoadingSetter),
        T::InstanceAccessor("complete", &T::CompleteGetter, nullptr),
        T::InstanceMethod("load", &T::LoadMethod),
      });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  thread_local Napi::FunctionReference *HTMLModelElement::constructor;
  void HTMLModelElement::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLModelElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    auto global = env.Global();
    global.Set("HTMLModelElement", func);
  }

  // Property implementations
  Napi::Value HTMLModelElement::SrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->src());
  }

  void HTMLModelElement::SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    this->node->setSrc(value.As<Napi::String>().Utf8Value());
  }

  Napi::Value HTMLModelElement::TypeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    auto type = this->node->type();
    return type.has_value() ? Napi::String::New(env, type.value()) : env.Null();
  }

  void HTMLModelElement::TypeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    if (value.IsNull() || value.IsUndefined())
    {
      // Clear the type hint to use auto-detection
      return;
    }
    this->node->setType(value.As<Napi::String>().Utf8Value());
  }

  Napi::Value HTMLModelElement::AutoplayGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, this->node->autoplay());
  }

  void HTMLModelElement::AutoplaySetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    this->node->setAutoplay(value.As<Napi::Boolean>().Value());
  }

  Napi::Value HTMLModelElement::LoadingGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->loadingString());
  }

  void HTMLModelElement::LoadingSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    this->node->setLoadingString(value.As<Napi::String>().Utf8Value());
  }

  Napi::Value HTMLModelElement::CompleteGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::Boolean::New(env, this->node->complete);
  }

  Napi::Value HTMLModelElement::LoadMethod(const Napi::CallbackInfo &info)
  {
    this->node->loadModel();
    return info.Env().Undefined();
  }
}