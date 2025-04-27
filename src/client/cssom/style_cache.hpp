#pragma once

#include <memory>
#include <unordered_map>
#include <client/html/html_element.hpp>
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
    /**
     * Creates a new style for the given element.
     * 
     * @param element The element to create the style for.
     * @param useElementStyle Whether to use the element's style as a base for the new style.
     * @returns The new style.
     */
    std::shared_ptr<CSSStyleDeclaration> createStyle(std::shared_ptr<dom::HTMLElement> element, bool useElementStyle = true)
    {
      assert(element != nullptr);

      std::shared_ptr<CSSStyleDeclaration> newStyle = nullptr;
      if (useElementStyle)
        newStyle = std::make_shared<CSSStyleDeclaration>(element->style());
      else
        newStyle = std::make_shared<CSSStyleDeclaration>();

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
