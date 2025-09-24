#include "./html_head_element.hpp"
#include <client/dom/html_head_element.hpp>

namespace script_bindings
{
  namespace html
  {
    using namespace v8;

    void HTMLHeadElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Head-specific properties
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "profile").ToLocalChecked(), ProfileGetter, ProfileSetter);
    }

    Local<Object> HTMLHeadElement::NewInstance(Isolate *isolate, std::shared_ptr<dom::HTMLHeadElement> nativeHeadElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeHeadElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLHeadElementBase::NewInstance(isolate, nativeHeadElement).As<Object>());
      }
    }

    Local<Function> HTMLHeadElement::Initialize(Isolate *isolate)
    {
      return HTMLHeadElementBase::Initialize(isolate);
    }

    HTMLHeadElement::HTMLHeadElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLHeadElement::ProfileGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLHeadElement *wrapper = ObjectWrap::Unwrap<HTMLHeadElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string profile = wrapper->GetNativeInstance()->getProfile();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, profile.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    // Property setters
    void HTMLHeadElement::ProfileSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLHeadElement *wrapper = ObjectWrap::Unwrap<HTMLHeadElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value profile(isolate, value);
        wrapper->GetNativeInstance()->setProfile(*profile);
      }
    }
  }
}