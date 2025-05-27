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

  shared_ptr<ComputedStyle> StyleCache::createStyle(shared_ptr<dom::Node> elementOrTextNode, bool useElementStyle)
  {
    assert(elementOrTextNode != nullptr);

    shared_ptr<ComputedStyle> newStyle = nullptr;
    if (useElementStyle && elementOrTextNode->isHTMLElement())
    {
      auto element = dynamic_pointer_cast<dom::HTMLElement>(elementOrTextNode);
      assert(element != nullptr && "The element must be an HTMLElement");
      newStyle = make_shared<ComputedStyle>(element->style(), values::computed::Context::From(element));
    }
    else
    {
      newStyle = make_shared<ComputedStyle>();
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
