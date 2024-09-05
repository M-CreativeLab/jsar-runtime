#include "./element.hpp"

namespace dombinding
{
  Napi::FunctionReference *Element::constructor;

  vector<Napi::ClassPropertyDescriptor<Element>> Element::GetClassProperties()
  {
    auto props = NodeBase<Element, dom::Element>::GetClassProperties();
    auto documentProps = vector<Napi::ClassPropertyDescriptor<Element>>(
        {
            Element::InstanceAccessor("className", &Element::ClassNameGetter, &Element::ClassNameSetter),
            Element::InstanceAccessor("id", &Element::IdGetter, &Element::IdSetter),
            Element::InstanceMethod("after", &Element::After),
            Element::InstanceMethod("animate", &Element::Animate),
            Element::InstanceMethod("append", &Element::Append),
            Element::InstanceMethod("attachShadow", &Element::AttachShadow),
            Element::InstanceMethod("before", &Element::Before),
            Element::InstanceMethod("checkVisibility", &Element::CheckVisibility),
            Element::InstanceMethod("closest", &Element::Closest),
        });
    props.insert(props.end(), documentProps.begin(), documentProps.end());
    return props;
  }

  void Element::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "Element", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Element", func);
  }

  Element::Element(const Napi::CallbackInfo &info) : NodeBase(info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    auto jsThis = info.This().As<Napi::Object>();
    jsThis.Set("localName", Napi::String::New(env, this->node->localName));
    jsThis.Set("namespaceURI", Napi::String::New(env, this->node->namespaceURI));
    jsThis.Set("prefix", Napi::String::New(env, this->node->prefix));
    jsThis.Set("tagName", Napi::String::New(env, this->node->tagName));
  }

  Element::~Element()
  {
  }

  Napi::Value Element::ClassNameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->className);
  }

  void Element::ClassNameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setClassName(value.As<Napi::String>().Utf8Value());
  }

  Napi::Value Element::IdGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return Napi::String::New(env, this->node->id);
  }

  void Element::IdSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    this->node->setId(value.As<Napi::String>().Utf8Value());
  }

  Napi::Value Element::After(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::Animate(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::Append(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::AttachShadow(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::Before(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::CheckVisibility(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::Closest(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    return env.Undefined();
  }

  Napi::Value Element::GetAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto attrName = info[0].ToString().Utf8Value();
    auto attrValue = this->node->getAttribute(attrName);
    return Napi::String::New(env, attrValue);
  }

  Napi::Value Element::GetAttributeNames(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Array result = Napi::Array::New(env);
    auto attrNames = this->node->getAttributeNames();
    for (size_t i = 0; i < attrNames.size(); i++)
      result[i] = Napi::String::New(env, attrNames[i]);
    return result;
  }

  Napi::Value Element::HasAttribute(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() < 1)
    {
      Napi::TypeError::New(env, "1 argument required, but only 0 present.").ThrowAsJavaScriptException();
      return env.Undefined();
    }
    auto attrName = info[0].ToString().Utf8Value();
    auto hasAttr = this->node->hasAttribute(attrName);
    return Napi::Boolean::New(env, hasAttr);
  }

  Napi::Value Element::HasAttributes(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, this->node->hasAttributes());
  }
}
