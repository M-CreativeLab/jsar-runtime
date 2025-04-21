#include <assert.h>
#include <client/scripting_base/v8_utils.hpp>

#include "./css_style_declaration.hpp"

namespace cssombinding
{
  using namespace std;
  using namespace v8;

  string kebab_to_camel_case(const string &kebabStr)
  {
    string camelStr;
    bool capitalizeNext = false;

    for (char c : kebabStr)
    {
      if (c == '-')
      {
        capitalizeNext = true;
      }
      else
      {
        if (capitalizeNext)
        {
          camelStr += static_cast<char>(std::toupper(c));
          capitalizeNext = false;
        }
        else
        {
          camelStr += c;
        }
      }
    }

    return camelStr;
  }

  string camel_to_kebab_case(const string &camelStr)
  {
    string kebabStr;
    for (char c : camelStr)
    {
      if (std::isupper(c))
      {
        if (!kebabStr.empty())
        {
          kebabStr += '-';
        }
        kebabStr += static_cast<char>(std::tolower(c));
      }
      else
      {
        kebabStr += c;
      }
    }
    return kebabStr;
  }

  void CSSStyleDeclaration::Init(Napi::Env env)
  {
    Napi::HandleScope scope(env);
    v8::Local<v8::Function> constructor = Base::Initialize(v8::Isolate::GetCurrent());

    env.Global().Set(
        Napi::String::New(env, "CSSStyleDeclaration"),
        scripting_base::Value(constructor));
  }

  Napi::Value CSSStyleDeclaration::NewInstance(Napi::Env env, shared_ptr<client_cssom::CSSStyleDeclaration> inner)
  {
    Napi::EscapableHandleScope scope(env);
    v8::Local<v8::Value> jsValue = Base::NewInstance(env, inner);

    // Convert the v8::Value to a Napi::Value
    napi_value value;
    memcpy(&value, &jsValue, sizeof(jsValue));
    return scope.Escape(value);
  }

  void CSSStyleDeclaration::GetPropertyPriority(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    v8::Local<v8::String> property = args[0].As<v8::String>();
    v8::String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::GetPropertyPriority: instance is null");

    auto priority = instance->inner()->getPropertyPriority(*propertyName);
    if (priority == client_cssom::CSSPropertyPriority::Important)
    {
      args.GetReturnValue()
          .Set(v8::String::NewFromUtf8(isolate, "important").ToLocalChecked());
    }
    else
    {
      args.GetReturnValue().Set(v8::String::Empty(isolate));
    }
  }

  void CSSStyleDeclaration::GetPropertyValue(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    v8::Local<v8::String> property = args[0].As<v8::String>();
    v8::String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::GetPropertyValue: instance is null");

    auto value = instance->inner()->getPropertyValue(*propertyName);
    if (value.empty())
    {
      args.GetReturnValue()
          .Set(v8::String::Empty(isolate));
    }
    else
    {
      args.GetReturnValue()
          .Set(v8::String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::Item(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsNumber())
    {
      isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    auto index = args[0].As<v8::Number>()->Value();
    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::Item: instance is null");

    auto value = instance->inner()->item(index);
    args.GetReturnValue().Set(v8::String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
  }

  void CSSStyleDeclaration::RemoveProperty(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    if (args.Length() != 1 || !args[0]->IsString())
    {
      isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    auto property = args[0].As<v8::String>();
    v8::String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::RemoveProperty: instance is null");

    auto result = instance->inner()->removeProperty(*propertyName);
    if (result.empty())
    {
      args.GetReturnValue().Set(v8::Undefined(isolate));
    }
    else
    {
      args.GetReturnValue()
          .Set(v8::String::NewFromUtf8(isolate, result.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::SetProperty(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(v8::String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    v8::Local<v8::String> property = args[0].As<v8::String>();
    v8::Local<v8::String> value;
    if (args.Length() >= 2 && args[1]->IsString())
      value = args[1].As<v8::String>();
    else
      value = v8::String::Empty(isolate);

    client_cssom::CSSPropertyPriority priority = client_cssom::CSSPropertyPriority::Normal;
    if (args.Length() >= 3)
    {
      if (args[2]->IsString())
      {
        v8::String::Utf8Value priorityValue(isolate, args[2]);
        if (strcmp(*priorityValue, "important") == 0)
          priority = client_cssom::CSSPropertyPriority::Important;
      }
    }

    v8::String::Utf8Value propertyName(isolate, property);
    v8::String::Utf8Value propertyValue(isolate, value);

    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::SetProperty: instance is null");

    instance->inner()->setProperty(*propertyName, *propertyValue, priority);
    args.GetReturnValue().Set(v8::Undefined(isolate));
  }

  void CSSStyleDeclaration::ToString(const v8::FunctionCallbackInfo<v8::Value> &args)
  {
    v8::Isolate *isolate = args.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();
    v8::HandleScope scope(isolate);

    auto instance = Unwrap(args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::ToString: instance is null");

    auto cssText = instance->inner()->cssText();
    args.GetReturnValue()
        .Set(v8::String::NewFromUtf8(isolate, cssText.c_str()).ToLocalChecked());
  }

  void CSSStyleDeclaration::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
  {
    Base::ConfigureFunctionTemplate(isolate, tpl);

    tpl->InstanceTemplate()->SetHandler(v8::NamedPropertyHandlerConfiguration(
        PropertyGetter,         // Getter
        PropertySetter,         // Setter
        nullptr,                // Query
        PropertyDeleter,        // Deleter
        PropertyEnumerator,     // Enumerator
        nullptr,                // Definer
        nullptr,                // Descriptor
        v8::Local<v8::Value>(), // Data
        v8::PropertyHandlerFlags::kNone));
  }

  void CSSStyleDeclaration::PropertyGetter(v8::Local<v8::Name> property,
                                           const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<v8::Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(v8::Undefined(isolate));
      return;
    }
    assert(property->IsString());

    // .constructor
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "constructor").ToLocalChecked()))
    {
      info.GetReturnValue().Set(Base::ConstructorFunction(isolate));
      return;
    }

    // .cssText
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "cssText").ToLocalChecked()))
    {
      auto cssText = instance->inner()->cssText();
      info.GetReturnValue()
          .Set(v8::String::NewFromUtf8(isolate, cssText.c_str()).ToLocalChecked());
      return;
    }

    // .length
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "length").ToLocalChecked()))
    {
      auto length = instance->inner()->length();
      info.GetReturnValue().Set(v8::Number::New(isolate, length));
      return;
    }

    // .item(index)
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "item").ToLocalChecked()))
    {
      info.GetReturnValue()
          .Set(v8::FunctionTemplate::New(isolate, &CSSStyleDeclaration::Item)
                   ->GetFunction(context)
                   .ToLocalChecked());
      return;
    }

    // .getPropertyValue(property)
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "getPropertyValue").ToLocalChecked()))
    {
      info.GetReturnValue()
          .Set(v8::FunctionTemplate::New(isolate, &CSSStyleDeclaration::GetPropertyValue)
                   ->GetFunction(context)
                   .ToLocalChecked());
      return;
    }

    // .getPropertyPriority(property)
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "getPropertyPriority").ToLocalChecked()))
    {
      info.GetReturnValue()
          .Set(v8::FunctionTemplate::New(isolate, &CSSStyleDeclaration::GetPropertyPriority)
                   ->GetFunction(context)
                   .ToLocalChecked());
      return;
    }

    // .removeProperty(property)
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "removeProperty").ToLocalChecked()))
    {
      info.GetReturnValue()
          .Set(v8::FunctionTemplate::New(isolate, &CSSStyleDeclaration::RemoveProperty)
                   ->GetFunction(context)
                   .ToLocalChecked());
      return;
    }

    // .setProperty(property, value)
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "setProperty").ToLocalChecked()))
    {
      info.GetReturnValue()
          .Set(v8::FunctionTemplate::New(isolate, &CSSStyleDeclaration::SetProperty)
                   ->GetFunction(context)
                   .ToLocalChecked());
      return;
    }

    // .cssFloat: alias for .float
    if (property->StrictEquals(v8::String::NewFromUtf8(isolate, "cssFloat").ToLocalChecked()))
    {
      auto floatValue = instance->inner()->getPropertyValue("float");
      if (floatValue.empty())
      {
        info.GetReturnValue()
            .Set(v8::Undefined(isolate));
        return;
      }
      else
      {
        info.GetReturnValue()
            .Set(v8::String::NewFromUtf8(isolate, floatValue.c_str()).ToLocalChecked());
        return;
      }
    }

    // Dynamic property getter for CSS properties
    v8::String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    auto value = instance->inner()->getPropertyValue(propertyNameStr);
    if (value.empty())
    {
      info.GetReturnValue()
          .Set(v8::Undefined(isolate));
    }
    else
    {
      info.GetReturnValue()
          .Set(v8::String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::PropertySetter(v8::Local<v8::Name> property, v8::Local<v8::Value> value,
                                           const v8::PropertyCallbackInfo<v8::Value> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<v8::Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(v8::Undefined(isolate));
      return;
    }
    assert(property->IsString());

    // Dynamic property getter for CSS properties
    v8::String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    v8::String::Utf8Value propertyValue(isolate, value);
    instance->inner()->setProperty(propertyNameStr, *propertyValue);
    info.GetReturnValue().Set(v8::Undefined(isolate));
  }

  void CSSStyleDeclaration::PropertyDeleter(v8::Local<v8::Name> property,
                                            const v8::PropertyCallbackInfo<v8::Boolean> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<v8::Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(v8::Boolean::New(isolate, false));
      return;
    }
    assert(property->IsString());

    // Dynamic property getter for CSS properties
    v8::String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    auto removedName = instance->inner()->removeProperty(propertyNameStr);
    info.GetReturnValue().Set(v8::Boolean::New(isolate, !removedName.empty()));
  }

  void CSSStyleDeclaration::PropertyEnumerator(const v8::PropertyCallbackInfo<v8::Array> &info)
  {
    v8::Isolate *isolate = info.GetIsolate();
    v8::Local<v8::Context> context = isolate->GetCurrentContext();

    v8::Local<v8::Array> keys = v8::Array::New(isolate, 0);
    CSSStyleDeclaration *instance = Unwrap(info.Holder());
    if (instance != nullptr)
    {
      auto decls = instance->inner();
      for (size_t index = 0; index < decls->length(); index++)
      {
        auto key = v8::String::NewFromUtf8(isolate,
                                           kebab_to_camel_case(decls->item(index)).c_str())
                       .ToLocalChecked();
        keys->Set(context, index, key).FromJust();
      }
    }

    // Append accessors
    keys->Set(context, keys->Length(), v8::String::NewFromUtf8(isolate, "cssFloat").ToLocalChecked())
        .FromJust();
    keys->Set(context, keys->Length(), v8::String::NewFromUtf8(isolate, "cssText").ToLocalChecked())
        .FromJust();
    keys->Set(context, keys->Length(), v8::String::NewFromUtf8(isolate, "length").ToLocalChecked())
        .FromJust();

    info.GetReturnValue().Set(keys);
  }
}
