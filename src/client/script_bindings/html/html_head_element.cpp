#include "./html_head_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace std;
    using namespace v8;

    void HTMLHeadElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    HTMLHeadElement::HTMLHeadElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadElementBase(isolate, args, true)
    {
    }
  }
}
