#include "./html_style_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLStyleElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  HTMLStyleElement::HTMLStyleElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLStyleElementBase(isolate, args)
  {
  }
}
