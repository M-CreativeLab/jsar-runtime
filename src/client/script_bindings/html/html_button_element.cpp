#include "./html_button_element.hpp"
#include <client/html/html_button_element.hpp>

namespace script_bindings
{
  namespace html_bindings
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        bool disabled = wrapper->inner()->disabled;
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        std::string type = wrapper->inner()->type;
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        std::string value = wrapper->inner()->value;
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        auto form = wrapper->inner()->form;
        if (form.empty())
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        std::string name = wrapper->inner()->name;
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
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        bool disabled = value->BooleanValue(isolate);
        wrapper->inner()->disabled = disabled;
      }
    }

    void HTMLButtonElement::TypeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner() && value->IsString())
      {
        String::Utf8Value type(isolate, value);
        wrapper->inner()->type = *type;
      }
    }

    void HTMLButtonElement::ValueSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner() && value->IsString())
      {
        String::Utf8Value val(isolate, value);
        wrapper->inner()->setNodeValue(*val);
      }
    }

    void HTMLButtonElement::NameSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner() && value->IsString())
      {
        String::Utf8Value name(isolate, value);
        wrapper->inner()->name = *name;
      }
    }

    // Methods
    void HTMLButtonElement::Click(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(info.This());
      
      if (wrapper && wrapper->inner())
      {
        wrapper->inner()->click();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid button element").ToLocalChecked()));
      }
    }
  }
}