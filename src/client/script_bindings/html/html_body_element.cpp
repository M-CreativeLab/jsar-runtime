#include "html_body_element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace html_bindings
  {
    HTMLBodyElement::HTMLBodyElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLBodyElementBase(isolate, args)
    {
    }

    void HTMLBodyElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope handle_scope(isolate);
      auto instance_template = tpl->InstanceTemplate();

      // Event handler properties
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "onload").ToLocalChecked(),
                                     OnLoadGetter,
                                     OnLoadSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "onunload").ToLocalChecked(),
                                     OnUnloadGetter,
                                     OnUnloadSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "onbeforeunload").ToLocalChecked(),
                                     OnBeforeUnloadGetter,
                                     OnBeforeUnloadSetter);
      instance_template->SetAccessor(String::NewFromUtf8(isolate, "onresize").ToLocalChecked(),
                                     OnResizeGetter,
                                     OnResizeSetter);
    }

    Local<Object> HTMLBodyElement::NewInstance(Isolate *isolate, shared_ptr<dom::HTMLBodyElement> nativeElement)
    {
      EscapableHandleScope scope(isolate);
      return nativeElement != nullptr
               ? scope.Escape(HTMLBodyElementBase::NewInstance(isolate, nativeElement).As<Object>())
               : scope.Escape(Local<Object>());
    }

    // Event handler property getters/setters
    void HTMLBodyElement::OnLoadGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get onload event handler
        info.GetReturnValue().Set(Null(isolate));
      }
    }

    void HTMLBodyElement::OnLoadSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Set onload event handler
      }
    }

    void HTMLBodyElement::OnUnloadGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get onunload event handler
        info.GetReturnValue().Set(Null(isolate));
      }
    }

    void HTMLBodyElement::OnUnloadSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Set onunload event handler
      }
    }

    void HTMLBodyElement::OnBeforeUnloadGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get onbeforeunload event handler
        info.GetReturnValue().Set(Null(isolate));
      }
    }

    void HTMLBodyElement::OnBeforeUnloadSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Set onbeforeunload event handler
      }
    }

    void HTMLBodyElement::OnResizeGetter(Local<String> property, const PropertyCallbackInfo<Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Get onresize event handler
        info.GetReturnValue().Set(Null(isolate));
      }
    }

    void HTMLBodyElement::OnResizeSetter(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void> &info)
    {
      Isolate *isolate = info.GetIsolate();
      auto element = Unwrap(isolate, info.This());
      if (element)
      {
        // TODO: Set onresize event handler
      }
    }
  }
}
