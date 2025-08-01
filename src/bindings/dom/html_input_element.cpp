#include <assert.h>
#include "./html_input_element.hpp"

namespace dombinding
{
  std::vector<Napi::ClassPropertyDescriptor<HTMLInputElement>> HTMLInputElement::GetClassProperties(Napi::Env env)
  {
    using T = HTMLInputElement;
    auto props = HTMLElementBase<HTMLInputElement, dom::HTMLInputElement>::GetClassProperties(env);
    auto added = std::vector<Napi::ClassPropertyDescriptor<HTMLInputElement>>(
      {
        // Properties
        T::InstanceAccessor("type", &T::TypeGetter, &T::TypeSetter),
        T::InstanceAccessor("value", &T::ValueGetter, &T::ValueSetter),
        T::InstanceAccessor("checked", &T::CheckedGetter, &T::CheckedSetter),
        T::InstanceAccessor("disabled", &T::DisabledGetter, &T::DisabledSetter),
        T::InstanceAccessor("required", &T::RequiredGetter, &T::RequiredSetter),
        T::InstanceAccessor("placeholder", &T::PlaceholderGetter, &T::PlaceholderSetter),
        T::InstanceAccessor("name", &T::NameGetter, &T::NameSetter),
        T::InstanceAccessor("valueAsNumber", &T::ValueAsNumberGetter, &T::ValueAsNumberSetter),
        
        // Methods
        T::InstanceMethod("checkValidity", &T::CheckValidity),
        T::InstanceMethod("setCustomValidity", &T::SetCustomValidity),
        T::InstanceMethod("reportValidity", &T::ReportValidity),
        T::InstanceMethod("stepUp", &T::StepUp),
        T::InstanceMethod("stepDown", &T::StepDown),
        T::InstanceMethod("select", &T::Select),
        T::InstanceMethod("setSelectionRange", &T::SetSelectionRange),
      });
    props.insert(props.end(), added.begin(), added.end());
    return props;
  }

  thread_local Napi::FunctionReference *HTMLInputElement::constructor;
  void HTMLInputElement::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "HTMLInputElement", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("HTMLInputElement", func);
  }

  // Property implementations
  Napi::Value HTMLInputElement::TypeGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->type());
  }

  void HTMLInputElement::TypeSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsString()) {
      node->setType(value.As<Napi::String>().Utf8Value());
    }
  }

  Napi::Value HTMLInputElement::ValueGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->value());
  }

  void HTMLInputElement::ValueSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsString()) {
      node->setValue(value.As<Napi::String>().Utf8Value());
    }
  }

  Napi::Value HTMLInputElement::CheckedGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->checked());
  }

  void HTMLInputElement::CheckedSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsBoolean()) {
      node->setChecked(value.As<Napi::Boolean>().Value());
    }
  }

  Napi::Value HTMLInputElement::DisabledGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->disabled());
  }

  void HTMLInputElement::DisabledSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsBoolean()) {
      node->setDisabled(value.As<Napi::Boolean>().Value());
    }
  }

  Napi::Value HTMLInputElement::RequiredGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->required());
  }

  void HTMLInputElement::RequiredSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsBoolean()) {
      node->setRequired(value.As<Napi::Boolean>().Value());
    }
  }

  Napi::Value HTMLInputElement::PlaceholderGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->placeholder());
  }

  void HTMLInputElement::PlaceholderSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsString()) {
      node->setPlaceholder(value.As<Napi::String>().Utf8Value());
    }
  }

  Napi::Value HTMLInputElement::NameGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::String::New(env, node->name());
  }

  void HTMLInputElement::NameSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsString()) {
      node->setName(value.As<Napi::String>().Utf8Value());
    }
  }

  Napi::Value HTMLInputElement::ValueAsNumberGetter(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    double value = node->valueAsNumber();
    if (std::isnan(value)) {
      return env.Null();
    }
    return Napi::Number::New(env, value);
  }

  void HTMLInputElement::ValueAsNumberSetter(const Napi::CallbackInfo &info, const Napi::Value &value)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (value.IsNumber()) {
      node->setValueAsNumber(value.As<Napi::Number>().DoubleValue());
    }
  }

  // Method implementations
  Napi::Value HTMLInputElement::CheckValidity(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->checkValidity());
  }

  Napi::Value HTMLInputElement::SetCustomValidity(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() > 0 && info[0].IsString()) {
      node->setCustomValidity(info[0].As<Napi::String>().Utf8Value());
    }
    return env.Undefined();
  }

  Napi::Value HTMLInputElement::ReportValidity(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    return Napi::Boolean::New(env, node->reportValidity());
  }

  Napi::Value HTMLInputElement::StepUp(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    int steps = 1;
    if (info.Length() > 0 && info[0].IsNumber()) {
      steps = info[0].As<Napi::Number>().Int32Value();
    }
    node->stepUp(steps);
    return env.Undefined();
  }

  Napi::Value HTMLInputElement::StepDown(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    int steps = 1;
    if (info.Length() > 0 && info[0].IsNumber()) {
      steps = info[0].As<Napi::Number>().Int32Value();
    }
    node->stepDown(steps);
    return env.Undefined();
  }

  Napi::Value HTMLInputElement::Select(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    node->select();
    return env.Undefined();
  }

  Napi::Value HTMLInputElement::SetSelectionRange(const Napi::CallbackInfo &info)
  {
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);
    if (info.Length() >= 2 && info[0].IsNumber() && info[1].IsNumber()) {
      int start = info[0].As<Napi::Number>().Int32Value();
      int end = info[1].As<Napi::Number>().Int32Value();
      node->setSelectionRange(start, end);
    }
    return env.Undefined();
  }
}
