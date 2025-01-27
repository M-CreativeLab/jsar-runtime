#pragma once

#include <memory>
#include <unordered_map>
#include <client/dom/html_element.hpp>
#include "./css_style_declaration.hpp"

namespace client_cssom
{
  class StyleCache : std::unordered_map<uint32_t, std::shared_ptr<CSSStyleDeclaration>>
  {
  public:
    StyleCache() = default;

  public:
    std::shared_ptr<CSSStyleDeclaration> findStyle(std::shared_ptr<dom::HTMLElement> element) const
    {
      if (TR_UNLIKELY(element == nullptr))
        return nullptr;

      auto it = find(element->uid);
      if (it != end())
        return it->second;
      return nullptr;
    }
    std::shared_ptr<CSSStyleDeclaration> createStyle(std::shared_ptr<dom::HTMLElement> element)
    {
      assert(element != nullptr);

      auto newStyle = std::make_shared<CSSStyleDeclaration>(element->style());
      assert(newStyle != nullptr);
      insert({element->uid, newStyle});
      return newStyle;
    }
    bool resetStyle(std::shared_ptr<dom::HTMLElement> element)
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
    inline void invalidateCache()
    {
      clear();
    }
  };
}
