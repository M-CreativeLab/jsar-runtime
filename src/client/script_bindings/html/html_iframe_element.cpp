#include "./html_iframe_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLIframeElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // No specific methods or properties for <html> element as of now
  }

  Local<Object> HTMLIframeElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLIframeElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLIframeElement::HTMLIframeElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLIframeElementBase(isolate, args)
  {
  }
}
