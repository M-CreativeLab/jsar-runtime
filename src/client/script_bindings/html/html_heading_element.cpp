#include "./html_heading_element.hpp"

namespace script_bindings
{
  namespace html_bindings
  {
    using namespace v8;

    void HTMLHeadingElement::ConfigureFunctionTemplate(Isolate *isolate, Local<FunctionTemplate> tpl)
    {
    }

    Local<Object> HTMLHeadingElement::NewInstance(Isolate *isolate, std::shared_ptr<dom::HTMLHeadingElement> nativeHeadingElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeHeadingElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLHeadingElementBase::NewInstance(isolate, nativeHeadingElement).As<Object>());
      }
    }

    Local<Function> HTMLHeadingElement::Initialize(Isolate *isolate)
    {
      return HTMLHeadingElementBase::Initialize(isolate);
    }

    HTMLHeadingElement::HTMLHeadingElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadingElementBase(isolate, args)
    {
    }
  }
}