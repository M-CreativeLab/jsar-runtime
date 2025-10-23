#include "./html_button_element.hpp"
#include <client/html/html_button_element.hpp>

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace v8;
    using namespace std;

    void HTMLButtonElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

      // Button-specific properties
      InstanceReadonlyAccessor(isolate, prototype, "form", &HTMLButtonElement::FormGetter);
      InstanceAccessor(isolate,
                       prototype,
                       "disabled",
                       &HTMLButtonElement::DisabledGetter,
                       &HTMLButtonElement::DisabledSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "type",
                       &HTMLButtonElement::TypeGetter,
                       &HTMLButtonElement::TypeSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "value",
                       &HTMLButtonElement::ValueGetter,
                       &HTMLButtonElement::ValueSetter);
      InstanceAccessor(isolate,
                       prototype,
                       "name",
                       &HTMLButtonElement::NameGetter,
                       &HTMLButtonElement::NameSetter);

      // Button methods
      InstanceMethod(isolate, prototype, "click", &HTMLButtonElement::Click);
    }

    HTMLButtonElement::HTMLButtonElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLButtonElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLButtonElement::DisabledGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        bool disabled = wrapper->handle()->disabled;
        info.GetReturnValue().Set(Boolean::New(isolate, disabled));
      }
      else
      {
        info.GetReturnValue().Set(Boolean::New(isolate, false));
      }
    }

    void HTMLButtonElement::TypeGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        std::string type = wrapper->handle()->type;
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, type.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "submit").ToLocalChecked());
      }
    }

    void HTMLButtonElement::ValueGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        std::string value = wrapper->handle()->value;
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, value.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    void HTMLButtonElement::FormGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        auto form = wrapper->handle()->form;
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

    void HTMLButtonElement::NameGetter(const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        std::string name = wrapper->handle()->name;
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    // Property setters
    void HTMLButtonElement::DisabledSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        bool disabled = value->BooleanValue(isolate);
        wrapper->handle()->disabled = disabled;
      }
    }

    void HTMLButtonElement::TypeSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsString())
      {
        String::Utf8Value type(isolate, value);
        wrapper->handle()->type = *type;
      }
    }

    void HTMLButtonElement::ValueSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsString())
      {
        String::Utf8Value val(isolate, value);
        wrapper->handle()->setNodeValue(*val);
      }
    }

    void HTMLButtonElement::NameSetter(Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle() && value->IsString())
      {
        String::Utf8Value name(isolate, value);
        wrapper->handle()->name = *name;
      }
    }

    // Methods
    void HTMLButtonElement::Click(const FunctionCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLButtonElement *wrapper = Unwrap(isolate, info.This());

      if (wrapper && wrapper->handle())
      {
        wrapper->handle()->click();
        info.GetReturnValue().SetUndefined();
      }
      else
      {
        isolate->ThrowException(Exception::Error(String::NewFromUtf8(isolate, "Invalid button element").ToLocalChecked()));
      }
    }
  }
}
