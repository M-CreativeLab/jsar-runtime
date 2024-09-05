#include <assert.h>
#include "./element.hpp"
#include "./html_html_element.hpp"
#include "./html_head_element.hpp"
#include "./html_body_element.hpp"
#include "./html_meta_element.hpp"

namespace dombinding
{
  Napi::FunctionReference *Element::constructor;
  void Element::Init(Napi::Env env)
  {
    auto props = GetClassProperties();
    Napi::Function func = DefineClass(env, "Element", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Element", func);
  }

  template <typename ObjectType = Element, typename ElementType = dom::Element>
  inline Napi::Object CreateTypedElementInternal(Napi::Env env, shared_ptr<dom::Element> element)
  {
    Napi::EscapableHandleScope scope(env);
    auto typedElement = dynamic_pointer_cast<ElementType>(element);
    auto external = Napi::External<ElementType>::New(env, typedElement.get());
    auto instance = ObjectType::constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

  Napi::Object CreateElement(Napi::Env env, shared_ptr<dom::Node> elementNode)
  {
    assert(elementNode->nodeType == dom::NodeType::ELEMENT_NODE);
    auto element = dynamic_pointer_cast<dom::Element>(elementNode);
    if (element->tagName == "HTML")
      return CreateTypedElementInternal<HTMLHtmlElement, dom::HTMLHtmlElement>(env, element);
    else if (element->tagName == "HEAD")
      return CreateTypedElementInternal<HTMLHeadElement, dom::HTMLHeadElement>(env, element);
    else if (element->tagName == "BODY")
      return CreateTypedElementInternal<HTMLBodyElement, dom::HTMLBodyElement>(env, element);
    else if (element->tagName == "META")
      return CreateTypedElementInternal<HTMLMetaElement, dom::HTMLMetaElement>(env, element);
    else
      return CreateTypedElementInternal(env, element);
  }
}
