#pragma once

#include <memory>

#include "./layout_box_model_object.hpp"
#include "./layout_object_child_list.hpp"

namespace client_layout
{
  /**
   * `LayoutInline` is the `LayoutObject` associated with "display: inline". This is called an "inline box" in CSS 2.1,
   * see: http://www.w3.org/TR/CSS2/visuren.html#inline-boxes.
   *
   * It is also the base class for content that behaves in similar way (like quotes and "display: ruby").
   *
   * Note that `LayoutInline` is always 'inline-level' but other `LayoutObject` can be 'inline-level', which is why it's
   * stored as a boolean on `LayoutObject`.
   *
   * Continuations can only be a `LayoutInline` or an anonymous `LayoutBlockFlow`. That's why continuations are handled
   * by `LayoutBoxModelObject` (common class between the 2). See `LayoutBoxModelObject::continuation` and
   * `setContinuation`.
   *
   * The given html `<b>Bold inline.<div>Bold block.</div>More bold inlines.</b>` will be parsed as:
   *
   * ```
   *   LayoutBlockFlow {HTML}
   *    LayoutBlockFlow {BODY}
   *      LayoutBlockFlow (anonymous)
   *        LayoutInline {B}
   *          LayoutText {#text}
   *            text run: "Bold inline."
   *      LayoutBlockFlow (anonymous)
   *        LayoutBlockFlow {DIV}
   *          LayoutText {#text}
   *            text run: "Bold block."
   *      LayoutBlockFlow (anonymous)
   *        LayoutInline {B}
   *          LayoutText {#text}
   *            text run: "More bold inlines."
   * ```
   */
  class LayoutInline : public LayoutBoxModelObject
  {
  public:
    LayoutInline(std::shared_ptr<dom::Node> node);

  public:
    const char *name() const override { return "LayoutInline"; }
    bool isLayoutInline() const override final { return true; }

    inline std::shared_ptr<LayoutObjectChildList> children() const { return children_; }
    inline std::shared_ptr<LayoutObjectChildList> children() { return children_; }
    inline LayoutObjectChildList &childrenRef() { return *children_; }
    inline const LayoutObjectChildList &childrenRef() const { return *children_; }

    std::shared_ptr<LayoutObject> firstChild() const;
    std::shared_ptr<LayoutObject> lastChild() const;

    // If you have a `LayoutInline`, use `firstChild` or `lastChild` instead.
    void slowFirstChild() const = delete;
    void slowLastChild() const = delete;

  private:
    std::shared_ptr<LayoutObjectChildList> virtualChildren() const override final { return children(); }
    std::shared_ptr<LayoutObjectChildList> virtualChildren() override final { return children(); }

  private:
    std::shared_ptr<LayoutObjectChildList> children_;
  };
}
