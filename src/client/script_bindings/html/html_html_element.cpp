#include "./html_html_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLHtmlElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // No specific methods or properties for <html> element as of now
  }

  Local<Object> HTMLHtmlElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLHtmlElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLHtmlElement::HTMLHtmlElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLHtmlElementBase(isolate, args)
  {
  }
}
