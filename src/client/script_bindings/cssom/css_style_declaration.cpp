#include <assert.h>
#include <client/scripting_base/v8_utils.hpp>
#include "./css_style_declaration.hpp"

namespace script_bindings::cssom_bindings
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

  void CSSStyleDeclaration::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    tpl->InstanceTemplate()->SetHandler(NamedPropertyHandlerConfiguration(
      PropertyGetter,     // Getter
      PropertySetter,     // Setter
      nullptr,            // Query
      PropertyDeleter,    // Deleter
      PropertyEnumerator, // Enumerator
      nullptr,            // Definer
      nullptr,            // Descriptor
      Local<Value>(),     // Data
      PropertyHandlerFlags::kNone));
  }

  void CSSStyleDeclaration::GetPropertyPriority(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    Local<String> property = args[0].As<String>();
    String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::GetPropertyPriority: instance is null");

    auto priority = instance->handle()->getPropertyPriority(*propertyName);
    if (priority == client_cssom::CSSPropertyPriority::Important)
    {
      args.GetReturnValue()
        .Set(String::NewFromUtf8(isolate, "important").ToLocalChecked());
    }
    else
    {
      args.GetReturnValue().Set(String::Empty(isolate));
    }
  }

  void CSSStyleDeclaration::GetPropertyValue(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    Local<String> property = args[0].As<String>();
    String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::GetPropertyValue: instance is null");

    auto value = instance->handle()->getPropertyValue(*propertyName);
    if (value.empty())
    {
      args.GetReturnValue()
        .Set(String::Empty(isolate));
    }
    else
    {
      args.GetReturnValue()
        .Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::Item(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsNumber())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    auto index = args[0].As<Number>()->Value();
    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::Item: instance is null");

    auto value = instance->handle()->item(index);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
  }

  void CSSStyleDeclaration::RemoveProperty(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    if (args.Length() != 1 || !args[0]->IsString())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    auto property = args[0].As<String>();
    String::Utf8Value propertyName(isolate, property);

    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::RemoveProperty: instance is null");

    auto result = instance->handle()->removeProperty(*propertyName);
    if (result.empty())
    {
      args.GetReturnValue().Set(Undefined(isolate));
    }
    else
    {
      args.GetReturnValue()
        .Set(String::NewFromUtf8(isolate, result.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::SetProperty(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    if (args.Length() < 1 || !args[0]->IsString())
    {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Invalid arguments").ToLocalChecked());
      return;
    }

    Local<String> property = args[0].As<String>();
    Local<String> value;
    if (args.Length() >= 2 && args[1]->IsString())
      value = args[1].As<String>();
    else
      value = String::Empty(isolate);

    client_cssom::CSSPropertyPriority priority = client_cssom::CSSPropertyPriority::Normal;
    if (args.Length() >= 3)
    {
      if (args[2]->IsString())
      {
        String::Utf8Value priorityValue(isolate, args[2]);
        if (strcmp(*priorityValue, "important") == 0)
          priority = client_cssom::CSSPropertyPriority::Important;
      }
    }

    String::Utf8Value propertyName(isolate, property);
    String::Utf8Value propertyValue(isolate, value);

    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::SetProperty: instance is null");

    instance->handle()->setProperty(*propertyName, *propertyValue, priority);
    args.GetReturnValue().Set(Undefined(isolate));
  }

  void CSSStyleDeclaration::ToString(const FunctionCallbackInfo<Value> &args)
  {
    Isolate *isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();
    HandleScope scope(isolate);

    auto instance = Unwrap(isolate, args.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::ToString: instance is null");

    auto cssText = instance->handle()->cssText();
    args.GetReturnValue()
      .Set(String::NewFromUtf8(isolate, cssText.c_str()).ToLocalChecked());
  }

  void CSSStyleDeclaration::PropertyGetter(Local<v8::Name> property,
                                           const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(isolate, info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(Undefined(isolate));
      return;
    }
    assert(property->IsString());

    // .constructor
    if (property->StrictEquals(String::NewFromUtf8(isolate, "constructor").ToLocalChecked()))
    {
      info.GetReturnValue().Set(GetConstructorFunction(isolate));
      return;
    }

    // .cssText
    if (property->StrictEquals(String::NewFromUtf8(isolate, "cssText").ToLocalChecked()))
    {
      auto cssText = instance->handle()->cssText();
      info.GetReturnValue()
        .Set(String::NewFromUtf8(isolate, cssText.c_str()).ToLocalChecked());
      return;
    }

    // .length
    if (property->StrictEquals(String::NewFromUtf8(isolate, "length").ToLocalChecked()))
    {
      auto length = instance->handle()->length();
      info.GetReturnValue().Set(Number::New(isolate, length));
      return;
    }

    // .item(index)
    if (property->StrictEquals(String::NewFromUtf8(isolate, "item").ToLocalChecked()))
    {
      info.GetReturnValue()
        .Set(FunctionTemplate::New(isolate, &CSSStyleDeclaration::Item)
               ->GetFunction(context)
               .ToLocalChecked());
      return;
    }

    // .getPropertyValue(property)
    if (property->StrictEquals(String::NewFromUtf8(isolate, "getPropertyValue").ToLocalChecked()))
    {
      info.GetReturnValue()
        .Set(FunctionTemplate::New(isolate, &CSSStyleDeclaration::GetPropertyValue)
               ->GetFunction(context)
               .ToLocalChecked());
      return;
    }

    // .getPropertyPriority(property)
    if (property->StrictEquals(String::NewFromUtf8(isolate, "getPropertyPriority").ToLocalChecked()))
    {
      info.GetReturnValue()
        .Set(FunctionTemplate::New(isolate, &CSSStyleDeclaration::GetPropertyPriority)
               ->GetFunction(context)
               .ToLocalChecked());
      return;
    }

    // .removeProperty(property)
    if (property->StrictEquals(String::NewFromUtf8(isolate, "removeProperty").ToLocalChecked()))
    {
      info.GetReturnValue()
        .Set(FunctionTemplate::New(isolate, &CSSStyleDeclaration::RemoveProperty)
               ->GetFunction(context)
               .ToLocalChecked());
      return;
    }

    // .setProperty(property, value)
    if (property->StrictEquals(String::NewFromUtf8(isolate, "setProperty").ToLocalChecked()))
    {
      info.GetReturnValue()
        .Set(FunctionTemplate::New(isolate, &CSSStyleDeclaration::SetProperty)
               ->GetFunction(context)
               .ToLocalChecked());
      return;
    }

    // .cssFloat: alias for .float
    if (property->StrictEquals(String::NewFromUtf8(isolate, "cssFloat").ToLocalChecked()))
    {
      auto floatValue = instance->handle()->getPropertyValue("float");
      if (floatValue.empty())
      {
        info.GetReturnValue()
          .Set(Undefined(isolate));
        return;
      }
      else
      {
        info.GetReturnValue()
          .Set(String::NewFromUtf8(isolate, floatValue.c_str()).ToLocalChecked());
        return;
      }
    }

    // Dynamic property getter for CSS properties
    String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    auto value = instance->handle()->getPropertyValue(propertyNameStr);
    if (value.empty())
    {
      info.GetReturnValue()
        .Set(Undefined(isolate));
    }
    else
    {
      info.GetReturnValue()
        .Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
    }
  }

  void CSSStyleDeclaration::PropertySetter(Local<v8::Name> property, Local<Value> value, const PropertyCallbackInfo<Value> &info)
  {
    Isolate *isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(isolate, info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(Undefined(isolate));
      return;
    }
    assert(property->IsString());

    // .cssFloat
    if (property->StrictEquals(String::NewFromUtf8(isolate, "cssFloat").ToLocalChecked()))
    {
      String::Utf8Value propertyValue(isolate, value);
      instance->handle()->setProperty("float", *propertyValue);
      info.GetReturnValue().Set(Undefined(isolate));
      return;
    }

    // .cssText
    if (property->StrictEquals(String::NewFromUtf8(isolate, "cssText").ToLocalChecked()))
    {
      String::Utf8Value cssTextValue(isolate, value);
      instance->handle()->setCssText(*cssTextValue);
      info.GetReturnValue().Set(Undefined(isolate));
      return;
    }

    // Dynamic property getter for CSS properties
    String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    String::Utf8Value propertyValue(isolate, value);
    instance->handle()->setProperty(propertyNameStr, *propertyValue);
    info.GetReturnValue().Set(Undefined(isolate));
  }

  void CSSStyleDeclaration::PropertyDeleter(Local<v8::Name> property,
                                            const PropertyCallbackInfo<Boolean> &info)
  {
    Isolate *isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    CSSStyleDeclaration *instance = Unwrap(isolate, info.Holder());
    assert(instance != nullptr && "CSSStyleDeclaration::PropertyGetter: instance is null");

    // scripting_base::console::Log(context, property.As<Value>());
    if (property->IsSymbol())
    {
      // TODO(yorkie): Handle symbol property, such as `Symbol.toStringTag`.
      info.GetReturnValue().Set(Boolean::New(isolate, false));
      return;
    }
    assert(property->IsString());

    // Dynamic property getter for CSS properties
    String::Utf8Value propertyName(isolate, property);
    string propertyNameStr(*propertyName, propertyName.length());
    // Convert camel-case to kebab-case if there is no hyphen
    if (propertyNameStr.find('-') == string::npos)
      propertyNameStr = camel_to_kebab_case(propertyNameStr);

    auto removedName = instance->handle()->removeProperty(propertyNameStr);
    info.GetReturnValue().Set(Boolean::New(isolate, !removedName.empty()));
  }

  void CSSStyleDeclaration::PropertyEnumerator(const PropertyCallbackInfo<Array> &info)
  {
    Isolate *isolate = info.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<Array> keys = Array::New(isolate, 0);
    CSSStyleDeclaration *instance = Unwrap(isolate, info.Holder());
    if (instance != nullptr)
    {
      auto decls = instance->handle();
      for (size_t index = 0; index < decls->length(); index++)
      {
        auto key = String::NewFromUtf8(isolate,
                                       kebab_to_camel_case(decls->item(index)).c_str())
                     .ToLocalChecked();
        keys->Set(context, index, key).FromJust();
      }
    }

    // Append accessors
    keys->Set(context, keys->Length(), String::NewFromUtf8(isolate, "cssFloat").ToLocalChecked())
      .FromJust();
    keys->Set(context, keys->Length(), String::NewFromUtf8(isolate, "cssText").ToLocalChecked())
      .FromJust();
    keys->Set(context, keys->Length(), String::NewFromUtf8(isolate, "length").ToLocalChecked())
      .FromJust();

    info.GetReturnValue().Set(keys);
  }
}
