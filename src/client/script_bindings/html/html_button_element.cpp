#include "./html_button_element.hpp"
#include <client/dom/html_button_element.hpp>

namespace script_bindings
{
  namespace html
  {
    using namespace v8;

    void HTMLButtonElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Button-specific properties
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "disabled").ToLocalChecked(), DisabledGetter, DisabledSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "type").ToLocalChecked(), TypeGetter, TypeSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "value").ToLocalChecked(), ValueGetter, ValueSetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "form").ToLocalChecked(), FormGetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "name").ToLocalChecked(), NameGetter, NameSetter);

      // Button methods
      Local<ObjectTemplate> prototypeTemplate = tpl->PrototypeTemplate();
      prototypeTemplate->Set(String::NewFromUtf8(isolate, "click").ToLocalChecked(),
                           FunctionTemplate::New(isolate, Click));
    }

    Local<Object> HTMLButtonElement::NewInstance(Isolate *isolate, std::shared_ptr<dom::HTMLButtonElement> nativeButtonElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeButtonElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLButtonElementBase::NewInstance(isolate, nativeButtonElement).As<Object>());
      }
    }

    Local<Function> HTMLButtonElement::Initialize(Isolate *isolate)
    {
      return HTMLButtonElementBase::Initialize(isolate);
    }

    HTMLButtonElement::HTMLButtonElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLButtonElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLButtonElement::DisabledGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        bool disabled = wrapper->GetNativeInstance()->getDisabled();
        info.GetReturnValue().Set(Boolean::New(isolate, disabled));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
      }
    }

    void HTMLButtonElement::TypeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string type = wrapper->GetNativeInstance()->getType();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, type.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "submit").ToLocalChecked());
      }
    }

    void HTMLButtonElement::ValueGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string value = wrapper->GetNativeInstance()->getValue();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLButtonElement::FormGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        auto form = wrapper->GetNativeInstance()->getForm();
        if (form)
        {
          // TODO: Return wrapped HTMLFormElement
          info.GetReturnValue().Set(Null(isolate));
        }
        else
        {
          info.GetReturnValue().Set(Null(isolate));
        }
      }
      else
      {
        info.GetReturnValue().Set(Null(isolate));
      }
    }

    void HTMLButtonElement::NameGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string name = wrapper->GetNativeInstance()->getName();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    // Property setters
    void HTMLButtonElement::DisabledSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        bool disabled = value->BooleanValue(isolate);
        wrapper->GetNativeInstance()->setDisabled(disabled);
      }
    }

    void HTMLButtonElement::TypeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value type(isolate, value);
        wrapper->GetNativeInstance()->setType(*type);
      }
    }

    void HTMLButtonElement::ValueSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value val(isolate, value);
        wrapper->GetNativeInstance()->setValue(*val);
      }
    }

    void HTMLButtonElement::NameSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value name(isolate, value);
        wrapper->GetNativeInstance()->setName(*name);
      }
    }

    // Methods
    void HTMLButtonElement::Click(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = ObjectWrap::Unwrap<HTMLButtonElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        wrapper->GetNativeInstance()->click();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid button element").ToLocalChecked()));
      }
    }
  }
}