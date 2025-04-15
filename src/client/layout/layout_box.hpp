#pragma once

#include <memory>
#include <common/math_utils.hpp>
#include <common/math3d/plane.hpp>
#include <client/dom/types.hpp>

#include "./geometry/bounding_box.hpp"
#include "./geometry/rect.hpp"
#include "./display_type.hpp"
#include "./layout_box_model_object.hpp"
#include "./overflow_model.hpp"

namespace client_layout
{
  /**
   * `LayoutBox` implements the full CSS box model.
   *
   * The CSS box model is based on a series of nested boxes: http://www.w3.org/TR/CSS21/box.html
   *
   *       |----------------------------------------------------|
   *       |                                                    |
   *       |                   margin-top                       |
   *       |                                                    |
   *       |     |-----------------------------------------|    |
   *       |     |                                         |    |
   *       |     |             border-top                  |    |
   *       |     |                                         |    |
   *       |     |    |--------------------------|----|    |    |
   *       |     |    |                          |    |    |    |
   *       |     |    |       padding-top        |####|    |    |
   *       |     |    |                          |####|    |    |
   *       |     |    |    |----------------|    |####|    |    |
   *       |     |    |    |                |    |    |    |    |
   *       | ML  | BL | PL |  content box   | PR | SW | BR | MR |
   *       |     |    |    |                |    |    |    |    |
   *       |     |    |    |----------------|    |    |    |    |
   *       |     |    |                          |    |    |    |
   *       |     |    |      padding-bottom      |    |    |    |
   *       |     |    |--------------------------|----|    |    |
   *       |     |    |                      ####|    |    |    |
   *       |     |    |     scrollbar height ####| SC |    |    |
   *       |     |    |                      ####|    |    |    |
   *       |     |    |-------------------------------|    |    |
   *       |     |                                         |    |
   *       |     |             border-bottom               |    |
   *       |     |                                         |    |
   *       |     |-----------------------------------------|    |
   *       |                                                    |
   *       |                   margin-bottom                    |
   *       |                                                    |
   *       |----------------------------------------------------|
   *
   * BL = border-left
   * BR = border-right
   * ML = margin-left
   * MR = margin-right
   * PL = padding-left
   * PR = padding-right
   * SC = scroll corner (contains UI for resizing (see the 'resize' property)
   * SW = scrollbar width
   *
   */
  class LayoutBox : public LayoutBoxModelObject
  {
  public:
    /**
     * Construct a box.
     *
     * @param node The element to be associated with the box.
     */
    LayoutBox(std::shared_ptr<dom::Node> node);

  public:
    const char *name() const override { return "LayoutBox"; }
    bool isBox() const override final { return true; }

    std::shared_ptr<LayoutBox> firstChildBox() const;
    std::shared_ptr<LayoutBox> lastChildBox() const;
    std::shared_ptr<LayoutBox> nextSiblingBox() const;
    std::shared_ptr<LayoutBox> previousSiblingBox() const;
    std::shared_ptr<LayoutBox> parentBox() const;

    virtual glm::vec3 size() const;

    // Returns the front face plane of the box in world space.
    math3d::TrPlane physicalBorderBoxFront() const;
    geometry::BoundingBox physicalBorderBoxRect() const;
    geometry::Rect<float> physicalPaddingBoxRect() const
    {
      return geometry::Rect<float>(clientLeft(),
                                   clientTop(),
                                   clientWidth(),
                                   clientHeight());
    }

    geometry::Rect<float> noOverflowRect() const { return physicalPaddingBoxRect(); }
    geometry::Rect<float> scrollableOverflowRect() const;

    // These methods don't mean the box *actually* has top/left overflow. They mean that *if* the box overflows, it will
    // overflow to the top/left rather than the bottom/right. This happens when child content is laid out right-to-left
    // (e.g. direction:rtl) or or bottom-to-top (e.g. direction:rtl writing-mode:vertical-rl).
    virtual bool hasTopOverflow() const;
    virtual bool hasLeftOverflow() const;

    virtual void updateAfterLayout();

    // Sets the scrollable-overflow from the current set of layout-results.
    void setScrollableOverflowFromLayoutResults();

    float contentLeft() const { return clientLeft() + borderLeft(); }
    float contentTop() const { return clientTop() + borderTop(); }
    float contentWidth() const
    {
      return transmute::common::math_utils::ClampNegativeToZero(clientWidth() - paddingLeft() - paddingRight());
    }
    float contentHeight() const
    {
      return transmute::common::math_utils::ClampNegativeToZero(clientHeight() - paddingTop() - paddingBottom());
    }
    glm::vec3 contentSize() const
    {
      return glm::vec3(contentWidth(), contentHeight(), 0.0f);
    }
    float contentLogicalWidth() const
    {
      return isHorizontalWritingMode()
                 ? contentWidth()
                 : contentHeight();
    }
    float contentLogicalHeight() const
    {
      return isHorizontalWritingMode()
                 ? contentHeight()
                 : contentWidth();
    }

    float clientLeft() const;
    float clientTop() const;

    float clientWidth() const;
    float clientHeight() const;
    float clientLogicalWidth() const
    {
      return isHorizontalWritingMode()
                 ? clientWidth()
                 : clientHeight();
    }
    float clientLogicalHeight() const
    {
      return isHorizontalWritingMode()
                 ? clientHeight()
                 : clientWidth();
    }

    virtual float scrollWidth() const;
    virtual float scrollHeight() const;

    bool isUserScrollable() const { return hasScrollableOverflowX() || hasScrollableOverflowY(); }
    virtual void autoScroll(const glm::vec3 &offset);
    void scrollTo(const glm::vec3 &offset);
    void scrollBy(const glm::vec3 &offset);
    bool scrollsOverflow() const;

    bool hasScrollableOverflowX() const { return scrollsOverflowX() && scrollWidth() != clientWidth(); }
    bool hasScrollableOverflowY() const { return scrollsOverflowY() && scrollHeight() != clientHeight(); }
    bool scrollsOverflowX() const;
    bool scrollsOverflowY() const;

    glm::vec3 scrollOrigin() const;
    glm::vec3 scrolledContentOffset() const;

    bool nodeAtPoint(HitTestResult &, const HitTestRay &, const glm::vec3 &accumulatedOffset,
                     HitTestPhase) override;
    // Returns if this box has overflow that is hit-testable.
    bool hasHitTestableOverflow() const;
    // Fast check if `NodeAtPoint` may find a hit based on the bounding box intersection.
    bool mayIntersect(const HitTestResult &, const HitTestRay &, const glm::vec3 &accumulatedOffset) const;

  protected:
    virtual bool hitTestChildren(HitTestResult &, const HitTestRay &, const glm::vec3 &accumulatedOffset,
                                 HitTestPhase);

    bool computeLayout(const ConstraintSpace &) override;
    void updateFromStyle() override;

  private:
    bool isHorizontalWritingMode() const;

    inline bool scrollableOverflowIsSet() const { return overflow_ != nullptr && overflow_->scrollableOverflow; }
    inline bool visualOverflowIsSet() const { return overflow_ != nullptr && overflow_->visualOverflow; }

    glm::vec3 computeSize() const;
    void invalidateCachedGeometry();

  protected:
    glm::vec3 frame_size_;

  private:
    std::shared_ptr<BoxOverflowModel> overflow_;
  };
}
