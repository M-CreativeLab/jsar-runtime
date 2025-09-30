#include "html_body_element.hpp"

using namespace std;
using namespace v8;

namespace script_bindings
{
  namespace html_bindings
  {
    HTMLBodyElement::HTMLBodyElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLBodyElementBase(isolate, args)
    {
    }

    void HTMLBodyElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }
  }
}
