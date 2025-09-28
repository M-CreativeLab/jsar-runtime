#include "./html_model_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLModelElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLModelElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLModelElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLModelElement::HTMLModelElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLModelElementBase(isolate, args)
  {
  }
}
