#include "./html_template_element.hpp"
#include <client/script_bindings/dom/document_fragment.hpp>

namespace endor
{
  namespace script_bindings::html_bindings
  {
    using namespace std;
    using namespace v8;

    void HTMLTemplateElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
      HandleScope scope(isolate);
      Local<ObjectTemplate> instance = tpl->PrototypeTemplate();

      InstanceReadonlyAccessor(isolate, instance, "content", &HTMLTemplateElement::ContentGetter);
    }

    void HTMLTemplateElement::ContentGetter(const v8::PropertyCallbackInfo<v8::Value> &info)
    {
      Isolate *isolate = info.GetIsolate();
      HandleScope scope(isolate);

      auto contentFragment = dom_bindings::DocumentFragment::GetOrNewInstance(isolate,
                                                                              handle()->getContent());
      info.GetReturnValue().Set(contentFragment);
    }
  }
} // namespace endor
