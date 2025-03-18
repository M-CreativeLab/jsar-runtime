#include <iostream>
#include <assert.h>

#include "./element-inl.hpp"
#include "./html_element-inl.hpp"
#include "./all_html_elements.hpp"

namespace dombinding
{
  using namespace std;
  using namespace Napi;

  thread_local FunctionReference *Element::constructor;
  void Element::Init(Napi::Env env)
  {
    auto props = GetClassProperties(env);
    Napi::Function func = DefineClass(env, "Element", props);
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.Global().Set("Element", func);
  }

  // Create a new instance of `Element` in JavaScript from a node-implementation object.
  template <typename ObjectType = HTMLElement, typename ElementType = dom::HTMLElement>
  inline Object MakeFromImpl(Napi::Env env, shared_ptr<dom::Element> element)
  {
    EscapableHandleScope scope(env);
    shared_ptr<ElementType> typedElement = dynamic_pointer_cast<ElementType>(element);
    if (typedElement == nullptr)
      throw runtime_error("Invalid element type");

    Value value = NodeBase<ObjectType, ElementType>::FromImpl(env, typedElement);
    return scope.Escape(value).ToObject();
  }

  struct ElementInit
  {
    string namespaceURI;
    string tagName;
    shared_ptr<dom::Document> ownerDocument;
  };

  // Create a new instance of `Element` in JavaScript from the init object.
  template <typename ObjectType = HTMLElement, typename ElementType = dom::HTMLElement>
  inline Object MakeFromInit(Napi::Env env, const ElementInit &init)
  {
    assert(init.ownerDocument != nullptr && "ownerDocument is required");
    shared_ptr<dom::Element> element = dom::Element::CreateElement(init.namespaceURI,
                                                                   init.tagName,
                                                                   init.ownerDocument);
    return MakeFromImpl<ObjectType, ElementType>(env, element);
  }

  Object Element::NewInstance(Napi::Env env, shared_ptr<dom::Node> elementNode)
  {
    assert(elementNode->nodeType == dom::NodeType::ELEMENT_NODE);
    auto element = dynamic_pointer_cast<dom::Element>(elementNode);

#define XX(tagNameStr, className)                                 \
  if (element->is(tagNameStr))                                    \
  {                                                               \
    return MakeFromImpl<className, dom::className>(env, element); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return MakeFromImpl(env, element).ToObject();
  }

  Object Element::NewInstance(Napi::Env env, string namespaceURI, string tagName,
                              shared_ptr<dom::Document> ownerDocument)
  {
    const ElementInit init = {namespaceURI, tagName, ownerDocument};
#define XX(tagNameStr, className)                              \
  if (tagName == tagNameStr)                                   \
  {                                                            \
    return MakeFromInit<className, dom::className>(env, init); \
  }
    TYPED_ELEMENT_MAP(XX)
#undef XX
    return MakeFromInit(env, init);
  }
}
