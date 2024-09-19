#include <iostream>
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
  inline Napi::Object CreateElementFromImpl(Napi::Env env, shared_ptr<dom::Element> element)
  {
    Napi::EscapableHandleScope scope(env);
    shared_ptr<ElementType> typedElement = dynamic_pointer_cast<ElementType>(element);
    NodeContainer<ElementType> elementContainer(typedElement);
    auto external = Napi::External<NodeContainer<ElementType>>::New(env, &elementContainer);
    auto instance = ObjectType::constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

  template <typename ObjectType = Element, typename ElementType = dom::Element>
  inline Napi::Object CreateElementFromNew(Napi::Env env, string namespaceURI, string tagName, weak_ptr<dom::Document> ownerDocument)
  {
    Napi::EscapableHandleScope scope(env);
    shared_ptr<ElementType> typedElement = dynamic_pointer_cast<ElementType>(dom::Element::CreateElement(namespaceURI, tagName, ownerDocument));
    NodeContainer<ElementType> elementContainer(typedElement);
    auto external = Napi::External<NodeContainer<ElementType>>::New(env, &elementContainer);
    auto instance = ObjectType::constructor->New({external});
    return scope.Escape(instance).ToObject();
  }

  Napi::Object Element::NewInstance(Napi::Env env, shared_ptr<dom::Node> elementNode)
  {
    assert(elementNode->nodeType == dom::NodeType::ELEMENT_NODE);
    auto element = dynamic_pointer_cast<dom::Element>(elementNode);

#define XX(tagNameStr, className)                                          \
  if (element->is(tagNameStr))                                             \
  {                                                                        \
    return CreateElementFromImpl<className, dom::className>(env, element); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return CreateElementFromImpl(env, element);
  }

  Napi::Object Element::NewInstance(Napi::Env env, string namespaceURI, string tagName, weak_ptr<dom::Document> ownerDocument)
  {
#define XX(tagNameStr, className)                                                                      \
  if (tagName == tagNameStr)                                                                           \
  {                                                                                                    \
    return CreateElementFromNew<className, dom::className>(env, namespaceURI, tagName, ownerDocument); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return CreateElementFromNew(env, namespaceURI, tagName, ownerDocument);
  }
}
