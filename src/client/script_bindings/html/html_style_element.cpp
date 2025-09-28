#include "./html_style_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLStyleElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLStyleElement::NewInstance(Isolate *isolate,
                                              shared_ptr<dom::HTMLStyleElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLStyleElement::HTMLStyleElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLStyleElementBase(isolate, args)
  {
  }
}
