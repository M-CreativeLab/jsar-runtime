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

    Local<Object> HTMLHeadElement::NewInstance(Isolate *isolate, shared_ptr<dom::HTMLHeadElement> nativeHeadElement)
    {
      EscapableHandleScope scope(isolate);
      if (nativeHeadElement == nullptr)
      {
        return scope.Escape(Local<Object>());
      }
      else
      {
        return scope.Escape(HTMLHeadElementBase::NewInstance(isolate, nativeHeadElement).As<Object>());
      }
    }

    Local<Function> HTMLHeadElement::Initialize(Isolate *isolate)
    {
      return HTMLHeadElementBase::Initialize(isolate);
    }

    HTMLHeadElement::HTMLHeadElement(Isolate *isolate, const FunctionCallbackInfo<Value> &args)
        : HTMLHeadElementBase(isolate, args)
    {
    }
  }
}