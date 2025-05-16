#include "./style_cache.hpp"

namespace client_cssom
{
  using namespace std;

  std::shared_ptr<ComputedStyle> StyleCache::findStyle(shared_ptr<dom::HTMLElement> element) const
  {
    if (TR_UNLIKELY(element == nullptr))
      return nullptr;

    auto it = find(element->uid);
    if (it != end())
      return it->second;
    return nullptr;
  }

  shared_ptr<ComputedStyle> StyleCache::createStyle(shared_ptr<dom::HTMLElement> element, bool useElementStyle)
  {
    assert(element != nullptr);

    shared_ptr<ComputedStyle> newStyle = nullptr;
    if (useElementStyle)
      newStyle = make_shared<ComputedStyle>(element->style());
    else
      newStyle = make_shared<ComputedStyle>();

    assert(newStyle != nullptr);
    insert({element->uid, newStyle});
    return newStyle;
  }

  bool StyleCache::resetStyle(shared_ptr<dom::HTMLElement> element)
  {
    if (TR_UNLIKELY(element == nullptr))
      return false;

    auto it = find(element->uid);
    if (it != end())
    {
      erase(it);
      return true;
    }
    return false;
  }
}
