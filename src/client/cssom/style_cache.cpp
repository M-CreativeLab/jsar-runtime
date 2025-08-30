#include <client/html/html_element.hpp>
#include <client/cssom/values/computed/context.hpp>

#include "./style_cache.hpp"

namespace client_cssom
{
  using namespace std;

  std::shared_ptr<ComputedStyle> StyleCache::findStyle(shared_ptr<dom::Node> elementOrTextNode) const
  {
    if (TR_UNLIKELY(elementOrTextNode == nullptr))
      return nullptr;

    auto it = find(elementOrTextNode->uid);
    if (it != end())
      return it->second;
    return nullptr;
  }

  shared_ptr<ComputedStyle> StyleCache::createStyle(shared_ptr<dom::Node> elementOrTextNode,
                                                    bool useElementStyle)
  {
    assert(elementOrTextNode != nullptr);

    shared_ptr<ComputedStyle> newStyle = nullptr;

    // Get parent style for CSS variable inheritance
    const ComputedStyle *parentStyle = nullptr;
    auto parentNode = elementOrTextNode->parentNode.lock();
    if (parentNode && parentNode->isElementOrText())
    {
      auto parentComputedStyle = findStyle(parentNode);
      if (parentComputedStyle)
      {
        parentStyle = parentComputedStyle.get();
      }
    }

    if (elementOrTextNode->isHTMLElement())
    {
      auto element = dynamic_pointer_cast<dom::HTMLElement>(elementOrTextNode);
      assert(element != nullptr && "The element must be an HTMLElement");

      // Use the new Make method with parent style inheritance
      auto declaredStyle = useElementStyle ? element->style() : element->defaultStyle();
      if (parentStyle)
      {
        auto computedStyle = ComputedStyle::Make(declaredStyle, element, parentStyle);
        newStyle = make_shared<ComputedStyle>(computedStyle);
      }
      else
      {
        newStyle = make_shared<ComputedStyle>(declaredStyle, values::computed::Context::From(element));
      }
    }
    else if (elementOrTextNode->isText())
    {
      auto textNode = dynamic_pointer_cast<dom::Text>(elementOrTextNode);

      // Use the new Make method with parent style inheritance
      if (parentStyle)
      {
        auto computedStyle = ComputedStyle::Make(textNode->defaultStyle(), textNode, parentStyle);
        newStyle = make_shared<ComputedStyle>(computedStyle);
      }
      else
      {
        newStyle = make_shared<ComputedStyle>(textNode->defaultStyle(), values::computed::Context::From(textNode));
      }
    }
    else
    {
      assert(false && "Only HTMLElement or Text node can be used to create a style.");
      return nullptr;
    }

    assert(newStyle != nullptr);
    insert({elementOrTextNode->uid, newStyle});
    return newStyle;
  }

  bool StyleCache::resetStyle(shared_ptr<dom::Node> elementOrTextNode)
  {
    if (TR_UNLIKELY(elementOrTextNode == nullptr))
      return false;

    auto it = find(elementOrTextNode->uid);
    if (it != end())
    {
      erase(it);
      return true;
    }
    return false;
  }
}
