#include "./html_paragraph_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLParagraphElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  HTMLParagraphElement::HTMLParagraphElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLParagraphElementBase(isolate, args)
  {
  }
}
