#include "./html_script_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLScriptElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLScriptElement::NewInstance(Isolate *isolate,
                                               shared_ptr<dom::HTMLScriptElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLScriptElement::HTMLScriptElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLScriptElementBase(isolate, args)
  {
  }
}
