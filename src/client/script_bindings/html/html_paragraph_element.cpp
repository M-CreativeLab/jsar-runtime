#include "./html_paragraph_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLParagraphElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLParagraphElement::NewInstance(Isolate *isolate,
                                                  shared_ptr<dom::HTMLParagraphElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLParagraphElement::HTMLParagraphElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLParagraphElementBase(isolate, args)
  {
  }
}
