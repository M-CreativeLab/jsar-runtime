#include "html_body_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    HTMLBodyElement::HTMLBodyElement(v8::Isolate *isolate, const v8::FunctionCallbackInfo<v8::Value> &args)
        : HTMLBodyElementBase(isolate, args)
    {
    }

    void HTMLBodyElement::ConfigureFunctionTemplate(v8::Isolate *isolate, v8::Local<v8::FunctionTemplate> tpl)
    {
      v8::HandleScope handle_scope(isolate);
      auto instance_template = tpl->InstanceTemplate();

      // Event handler properties
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "onload").ToLocalChecked(),
                                     OnLoadGetter,
                                     OnLoadSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "onunload").ToLocalChecked(),
                                     OnUnloadGetter,
                                     OnUnloadSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "onbeforeunload").ToLocalChecked(),
                                     OnBeforeUnloadGetter,
                                     OnBeforeUnloadSetter);
      instance_template->SetAccessor(v8::String::NewFromUtf8(isolate, "onresize").ToLocalChecked(),
                                     OnResizeGetter,
                                     OnResizeSetter);
    }

    v8::Local<v8::Object> HTMLBodyElement::NewInstance(v8::Isolate *isolate, std::shared_ptr<dom::HTMLBodyElement> nativeElement)
    {
      v8::EscapableHandleScope handle_scope(isolate);
      auto context = isolate->GetCurrentContext();

      auto constructor = HTMLBodyElement::GetConstructorFunction(isolate);
      v8::Local<v8::Object> instance;

      if (constructor->NewInstance(context).ToLocal(&instance))
      {
        HTMLBodyElement::Wrap(isolate, instance, new HTMLBodyElement(isolate, v8::FunctionCallbackInfo<v8::Value>(nullptr, 0, nullptr)));
        // TODO: Set native element instance
      }

      return handle_scope.Escape(instance);
    }

    v8::Local<v8::Function> HTMLBodyElement::Initialize(v8::Isolate *isolate)
    {
      return HTMLBodyElementBase::Initialize(isolate);
    }

    // Event handler property getters/setters
    void HTMLBodyElement::OnLoadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get onload event handler
        info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
      }
    }

    void HTMLBodyElement::OnLoadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Set onload event handler
      }
    }

    void HTMLBodyElement::OnUnloadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get onunload event handler
        info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
      }
    }

    void HTMLBodyElement::OnUnloadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Set onunload event handler
      }
    }

    void HTMLBodyElement::OnBeforeUnloadGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get onbeforeunload event handler
        info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
      }
    }

    void HTMLBodyElement::OnBeforeUnloadSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Set onbeforeunload event handler
      }
    }

    void HTMLBodyElement::OnResizeGetter(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Get onresize event handler
        info.GetReturnValue().Set(v8::Null(info.GetIsolate()));
      }
    }

    void HTMLBodyElement::OnResizeSetter(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void> &info)
    {
      auto element = GetNativeInstance(info.Holder());
      if (element)
      {
        // TODO: Set onresize event handler
      }
    }
  }
}