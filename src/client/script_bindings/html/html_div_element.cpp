#include "./html_div_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace html
  {
    // static
    void HTMLDivElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      // HTMLDivElement doesn't add any specific properties beyond HTMLElement
      // In a full implementation, specific HTML elements would add their own properties
    }

    // static
    Local<Object> HTMLDivElement::NewInstance(Isolate *isolate, std::shared_ptr<::dom::HTMLDivElement> nativeElement)
    {
      EscapableHandleScope scope(isolate);

      if (nativeElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }

      return scope.Escape(scripting_base::ObjectWrap<HTMLDivElement, ::dom::HTMLDivElement, script_bindings::dom::HTMLElement>::NewInstance(isolate, nativeElement).As<Object>());
    }

    // static
    Local<Function> HTMLDivElement::Initialize(Isolate *isolate)
    {
      return scripting_base::ObjectWrap<HTMLDivElement, ::dom::HTMLDivElement, script_bindings::dom::HTMLElement>::Initialize(isolate);
    }

    HTMLDivElement::HTMLDivElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : scripting_base::ObjectWrap<HTMLDivElement, ::dom::HTMLDivElement, script_bindings::dom::HTMLElement>(isolate, args)
    {
    }
  }
}