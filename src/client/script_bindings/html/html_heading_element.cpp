#include "./html_heading_element.hpp"
#include <client/dom/html_heading_element.hpp>

namespace script_bindings
{
  namespace html
  {
    using namespace v8;

    void HTMLHeadingElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      Local<ObjectTemplate> instanceTemplate = tpl->InstanceTemplate();

      // Heading-specific properties
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "level").ToLocalChecked(), LevelGetter);
      instanceTemplate->SetAccessor(String::NewFromUtf8(isolate, "align").ToLocalChecked(), AlignGetter, AlignSetter);
    }

    Local<Object> HTMLHeadingElement::NewInstance(Isolate *isolate, std::shared_ptr<dom::HTMLHeadingElement> nativeHeadingElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeHeadingElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLHeadingElementBase::NewInstance(isolate, nativeHeadingElement).As<Object>());
      }
    }

    Local<Function> HTMLHeadingElement::Initialize(Isolate *isolate)
    {
      return HTMLHeadingElementBase::Initialize(isolate);
    }

    HTMLHeadingElement::HTMLHeadingElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadingElementBase(isolate, args)
    {
    }

    // Property getters
    void HTMLHeadingElement::LevelGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLHeadingElement *wrapper = ObjectWrap::Unwrap<HTMLHeadingElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        int level = wrapper->GetNativeInstance()->getLevel();
        info.GetReturnValue().Set(Integer::New(isolate, level));
      }
      else
      {
        // Default to h1 (level 1) if no native instance
        info.GetReturnValue().Set(Integer::New(isolate, 1));
      }
    }

    void HTMLHeadingElement::AlignGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLHeadingElement *wrapper = ObjectWrap::Unwrap<HTMLHeadingElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance())
      {
        std::string align = wrapper->GetNativeInstance()->getAlign();
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, align.c_str()).ToLocalChecked());
      }
      else
      {
        info.GetReturnValue().Set(String::NewFromUtf8(isolate, "").ToLocalChecked());
      }
    }

    // Property setters
    void HTMLHeadingElement::AlignSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      Local<Object> self = info.Holder();
      HTMLHeadingElement *wrapper = ObjectWrap::Unwrap<HTMLHeadingElement>(self);
      
      if (wrapper && wrapper->GetNativeInstance() && value->IsString())
      {
        String::Utf8Value align(isolate, value);
        wrapper->GetNativeInstance()->setAlign(*align);
      }
    }
  }
}