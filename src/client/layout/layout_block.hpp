#pragma once

#include <memory>

#include "./layout_box.hpp"
#include "./layout_object_child_list.hpp"

namespace client_layout
{
  /**
   * `LayoutBlock` is the class that is used by any `LayoutObject` that is a containing block.
   *
   * @see http://www.w3.org/TR/CSS2/visuren.html#containing-block
   */
  class LayoutBlock : public LayoutBox
  {
    friend class LayoutView;

  public:
    LayoutBlock(std::shared_ptr<dom::Node> node);

  public:
    const char *name() const override
    {
      return "LayoutBlock";
    }
    bool isLayoutBlock() const override final
    {
      return true;
    }

    inline std::shared_ptr<LayoutObjectChildList> children() const
    {
      return children_;
    }
    inline std::shared_ptr<LayoutObjectChildList> children()
    {
      return children_;
    }
    inline LayoutObjectChildList &childrenRef()
    {
      return *children_;
    }
    inline const LayoutObjectChildList &childrenRef() const
    {
      return *children_;
    }

    std::shared_ptr<LayoutObject> firstChild() const;
    std::shared_ptr<LayoutObject> lastChild() const;

    // If you have a `LayoutBlock`, use `firstChild` or `lastChild` instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

  private:
    std::shared_ptr<LayoutObjectChildList> virtualChildren() const override final
    {
      return children();
    }
    std::shared_ptr<LayoutObjectChildList> virtualChildren() override final
    {
      return children();
    }

  protected:
    std::shared_ptr<LayoutObjectChildList> children_;
  };
}
