#include "./html_iframe_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLIframeElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
    // No specific methods or properties for <html> element as of now
  }

  HTMLIframeElement::HTMLIframeElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLIframeElementBase(isolate, args)
  {
  }
}
