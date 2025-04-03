#pragma once

#include <memory>
#include <client/dom/types.hpp>

#include "./display_type.hpp"
#include "./layout_box_model_object.hpp"

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
  };
}
