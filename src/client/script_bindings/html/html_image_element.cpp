#include "./html_image_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLImageElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  HTMLImageElement::HTMLImageElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLImageElementBase(isolate, args)
  {
  }
}
