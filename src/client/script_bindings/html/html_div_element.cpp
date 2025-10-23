#include "./html_div_element.hpp"
#include <iostream>

using namespace std;
using namespace v8;

namespace endor
{
  namespace script_bindings
  {
    namespace html_bindings
    {
      // static
      void HTMLDivElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
      {
        HandleScope scope(isolate);
        // HTMLDivElement doesn't add any specific properties beyond HTMLElement
        // In a full implementation, specific HTML elements would add their own properties
      }

      HTMLDivElement::HTMLDivElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
          : HTMLDivElementBase(isolate, args)
      {
      }
    }
  }
} // namespace endor
