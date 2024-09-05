#include <assert.h>
#include "./element.hpp"
#include "./all_html_elements.hpp"

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

  template <typename ObjectType = Element, typename ElementType = dom::Element>
  inline Napi::Object CreateTypedElementInternal(Napi::Env env)
  {
    Napi::EscapableHandleScope scope(env);
    ElementType typedElement;
    auto external = Napi::External<ElementType>::New(env, &typedElement);
    auto instance = ObjectType::constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

#define TYPED_ELEMENT_MAP(XX) \
  XX("HTML", HTMLHtmlElement) \
  XX("HEAD", HTMLHeadElement) \
  XX("BODY", HTMLBodyElement) \
  XX("META", HTMLMetaElement) \
  XX("SCRIPT", HTMLScriptElement)

  Napi::Object CreateElement(Napi::Env env, shared_ptr<dom::Node> elementNode)
  {
    assert(elementNode->nodeType == dom::NodeType::ELEMENT_NODE);
    auto element = dynamic_pointer_cast<dom::Element>(elementNode);

#define XX(tagNameStr, className)                                               \
  if (element->tagName == tagNameStr)                                           \
  {                                                                             \
    return CreateTypedElementInternal<className, dom::className>(env, element); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return CreateTypedElementInternal(env, element);
  }

  Napi::Object CreateElement(Napi::Env env, string tagName)
  {
#define XX(tagNameStr, className)                                      \
  if (tagName == tagNameStr)                                           \
  {                                                                    \
    return CreateTypedElementInternal<className, dom::className>(env); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return CreateTypedElementInternal(env);
  }
}
