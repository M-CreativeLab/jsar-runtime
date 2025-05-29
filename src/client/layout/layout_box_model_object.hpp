#pragma once

#include <memory>
#include <client/dom/types.hpp>
#include <client/scroll/scrollable_area.hpp>

#include "./display_type.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
  using Edge = client_cssom::values::generics::Edge;

  /**
   * This class is the base class for all CSS objects.
   */
  class LayoutBoxModelObject : public LayoutObject
  {
  public:
    LayoutBoxModelObject(std::shared_ptr<dom::Node> node);

  public:
    /**
     * An element whose content is outside the scope of the CSS formatting model, such as an image or embedded document.
     * For example, the content of the HTML `img` element is often replaced by the image that its src attribute
     * designates.
     *
     * @returns `true` if this box is a replaced element, otherwise `false`.
     */
    inline bool isReplacedElement() const { return display_.isContents(); }

    /**
     * @returns `true` if this box is visible, otherwise `false`.
     */
    inline bool visible() const { return !display_.isNone(); }

    /**
     * Set the display type of the box.
     *
     * @param displayStr The new display type to set.
     */
    void setDisplay(const std::string &displayStr);
    void setDisplay(const DisplayType &display);

    std::shared_ptr<client_scroll::ScrollableArea> getScrollableArea() const;

    virtual void updateFromStyle();

    virtual float paddingTop() const { return getPaddingEdgeWidth(Edge::kTop); }
    virtual float paddingBottom() const { return getPaddingEdgeWidth(Edge::kBottom); }
    virtual float paddingLeft() const { return getPaddingEdgeWidth(Edge::kLeft); }
    virtual float paddingRight() const { return getPaddingEdgeWidth(Edge::kRight); }

    virtual float borderTop() const { return getBorderEdgeWidth(Edge::kTop); }
    virtual float borderBottom() const { return getBorderEdgeWidth(Edge::kBottom); }
    virtual float borderLeft() const { return getBorderEdgeWidth(Edge::kLeft); }
    virtual float borderRight() const { return getBorderEdgeWidth(Edge::kRight); }

    float borderWidth() const { return borderLeft() + borderRight(); }
    float borderHeight() const { return borderTop() + borderBottom(); }

  private:
    bool isBoxModelObject() const override final { return true; }

    float getPaddingEdgeWidth(Edge) const;
    float getBorderEdgeWidth(Edge) const;
    void styleDidChange() override;

  private:
    DisplayType display_;
    std::shared_ptr<client_scroll::ScrollableArea> scrollable_area_;
  };
}
