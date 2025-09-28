#include "./html_template_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLTemplateElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLTemplateElement::NewInstance(Isolate *isolate,
                                                 shared_ptr<dom::HTMLTemplateElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLTemplateElement::HTMLTemplateElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLTemplateElementBase(isolate, args)
  {
  }
}
