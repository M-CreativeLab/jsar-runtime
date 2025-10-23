#include "./html_heading_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace v8;

    void HTMLHeadingElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    HTMLHeadingElement::HTMLHeadingElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadingElementBase(isolate, args)
    {
    }
  }
}