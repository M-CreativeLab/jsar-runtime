#include "./html_link_element.hpp"

namespace endor
{
  namespace script_bindings::html_bindings
  {
    using namespace std;
    using namespace v8;

    void HTMLLinkElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    HTMLLinkElement::HTMLLinkElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLLinkElementBase(isolate, args)
    {
    }
  }
} // namespace endor
