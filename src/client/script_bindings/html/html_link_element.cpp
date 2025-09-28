#include "./html_link_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLLinkElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLLinkElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLLinkElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLLinkElement::HTMLLinkElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLLinkElementBase(isolate, args)
  {
  }
}
