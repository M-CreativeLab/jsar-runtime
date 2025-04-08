#pragma once

#include <memory>
#include <client/dom/types.hpp>

#include "./display_type.hpp"
#include "./layout_object.hpp"

namespace client_layout
{
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

  private:
    bool isBoxModelObject() const override final { return true; }

  private:
    DisplayType display_;
  };
}
