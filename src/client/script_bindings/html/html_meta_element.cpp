#include "./html_meta_element.hpp"

namespace script_bindings::html_bindings
{
  using namespace std;
  using namespace v8;

  void HTMLMetaElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
  {
  }

  Local<Object> HTMLMetaElement::NewInstance(Isolate *isolate,
                                             shared_ptr<dom::HTMLMetaElement> nativeElement)
  {
    assert(nativeElement != nullptr);
    return HTMLElement::NewInstance(isolate, nativeElement).As<Object>();
  }

  HTMLMetaElement::HTMLMetaElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
      : HTMLMetaElementBase(isolate, args)
  {
  }
}
