#pragma once

#include <memory>
#include <unordered_map>
#include <client/html/html_element.hpp>
#include "./computed_style.hpp"

namespace client_cssom
{
  class StyleCache : std::unordered_map<uint32_t, std::shared_ptr<ComputedStyle>>
  {
  public:
    StyleCache() = default;

  public:
    std::shared_ptr<ComputedStyle> findStyle(std::shared_ptr<dom::HTMLElement> element) const;
    std::shared_ptr<ComputedStyle> createStyle(std::shared_ptr<dom::HTMLElement> element,
                                               bool useElementStyle = true);
    bool resetStyle(std::shared_ptr<dom::HTMLElement> element);

    inline void invalidateCache() { clear(); }
  };
}
