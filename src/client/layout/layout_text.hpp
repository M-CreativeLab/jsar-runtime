#pragma once

#include <memory>
#include <client/dom/types.hpp>

#include "./layout_object.hpp"

namespace client_layout
{
  class LayoutText final : public LayoutObject
  {
  public:
    /**
     * Construct a text sequence.
     *
     * @param tree The box tree.
     * @param textNode The text node to be associated with the text sequence.
     */
    LayoutText(std::shared_ptr<dom::Text> textNode);

  public:
    const char *name() const override { return "LayoutText"; }
    bool isText() const override final { return true; }

    std::shared_ptr<dom::Text> textNode() const;
    virtual std::string plainText() const;
  };
}
