#include "./html_span_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLSpanElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLSpanElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLSpanElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLSpanElement::HTMLSpanElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLSpanElementBase(isolate, args)
  {
  }
}
