#include <assert.h>
#include "./html_image_element.hpp"

using namespace std;

namespace dombinding
{
  Napi::FunctionReference *HTMLImageElement::constructor;

  vector<Napi::ClassPropertyDescriptor<HTMLImageElement>> HTMLImageElement::GetClassProperties()
  {
    using T = HTMLImageElement;
    auto props = HTMLElementBase<HTMLImageElement, dom::HTMLImageElement>::GetClassProperties();
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLImageElement>>(
        {
            T::InstanceAccessor("currentSrc", &T::CurrentSrcGetter, nullptr),
            T::InstanceAccessor("src", &T::SrcGetter, &T::SrcSetter),
            T::InstanceAccessor("naturalWidth", &T::NaturalWidthGetter, nullptr),
            T::InstanceAccessor("naturalHeight", &T::NaturalHeightGetter, nullptr),
            T::InstanceAccessor("width", &T::WidthGetter, nullptr),
            T::InstanceAccessor("height", &T::HeightGetter, nullptr),
        });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  void HTMLImageElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "HTMLImageElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLImageElement", func);
  }

  HTMLImageElement::HTMLImageElement(const Napi::CallbackInfo &info)
      : HTMLElementBase(info),
        canvasbinding::ImageSourceWrap<dom::HTMLImageElement>()
  {
    dataImpl = node;
  }

  Napi::Value HTMLImageElement::CurrentSrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), node->currentSrc);
  }

  Napi::Value HTMLImageElement::SrcGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(info.Env(), node->getSrc());
  }

  void HTMLImageElement::SrcSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    string newSrc = value.ToString().Utf8Value();
    node->setSrc(newSrc);
  }

  Napi::Value HTMLImageElement::NaturalWidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->width());
  }

  Napi::Value HTMLImageElement::NaturalHeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->height());
  }

  Napi::Value HTMLImageElement::WidthGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->width());
  }

  Napi::Value HTMLImageElement::HeightGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Number::New(info.Env(), node->height());
  }
}
