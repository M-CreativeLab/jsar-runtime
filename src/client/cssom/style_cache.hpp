#pragma once

#include <memory>
#include <unordered_map>
#include <client/dom/node.hpp>
#include "./computed_style.hpp"

namespace client_cssom
{
  class StyleCache : std::unordered_map<uint32_t, std::shared_ptr<ComputedStyle>>
  {
  public:
    StyleCache() = default;

  public:
    std::shared_ptr<ComputedStyle> findStyle(std::shared_ptr<dom::Node> elementOrTextNode) const;
    std::shared_ptr<ComputedStyle> createStyle(std::shared_ptr<dom::Node> elementOrTextNode,
                                               bool useElementStyle = true);
    bool resetStyle(std::shared_ptr<dom::Node> elementOrTextNode);

    inline void invalidateCache() { clear(); }
  };
}
