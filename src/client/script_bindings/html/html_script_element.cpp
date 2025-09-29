#include "./html_script_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLScriptElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  HTMLScriptElement::HTMLScriptElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLScriptElementBase(isolate, args)
  {
  }
}
