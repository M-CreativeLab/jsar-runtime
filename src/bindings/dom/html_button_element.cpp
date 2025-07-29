#include <assert.h>
#include "./html_button_element.hpp"

using namespace std;

namespace dombinding
{
  vector<Napi::ClassPropertyDescriptor<HTMLButtonElement>> HTMLButtonElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLButtonElement;
    auto props = HTMLElementBase<HTMLButtonElement, dom::HTMLButtonElement>::GetClassProperties(env);
    auto added = vector<Napi::ClassPropertyDescriptor<HTMLButtonElement>>(
      {
        T::InstanceAccessor("disabled", &T::GetDisabled, &T::SetDisabled),
        T::InstanceAccessor("type", &T::GetType, &T::SetType),
        T::InstanceAccessor("form", &T::GetForm, &T::SetForm),
        T::InstanceAccessor("name", &T::GetName, &T::SetName),
        T::InstanceAccessor("value", &T::GetValue, &T::SetValue),
      });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  thread_local Napi::FunctionReference *HTMLButtonElement::constructor;
  void HTMLButtonElement::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLButtonElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLButtonElement", func);
  }

  Napi::Value HTMLButtonElement::GetDisabled(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->disabled);
  }

  void HTMLButtonElement::SetDisabled(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    node->disabled = value.ToBoolean().Value();

    // Update the attribute
    if (node->disabled)
    {
      node->setAttribute("disabled", "");
    }
    else
    {
      node->removeAttribute("disabled");
    }
  }

  Napi::Value HTMLButtonElement::GetType(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->type);
  }

  void HTMLButtonElement::SetType(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    string newType = value.ToString().Utf8Value();
    node->type = newType;
    node->setAttribute("type", newType);
  }

  Napi::Value HTMLButtonElement::GetForm(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->form);
  }

  void HTMLButtonElement::SetForm(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    string newForm = value.ToString().Utf8Value();
    node->form = newForm;
    node->setAttribute("form", newForm);
  }

  Napi::Value HTMLButtonElement::GetName(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->name);
  }

  void HTMLButtonElement::SetName(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    string newName = value.ToString().Utf8Value();
    node->name = newName;
    node->setAttribute("name", newName);
  }

  Napi::Value HTMLButtonElement::GetValue(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->value);
  }

  void HTMLButtonElement::SetValue(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    string newValue = value.ToString().Utf8Value();
    node->value = newValue;
    node->setAttribute("value", newValue);
  }
}
