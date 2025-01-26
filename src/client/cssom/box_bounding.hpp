#pragma once

#include <vector>
#include <optional>
#include <client/dom/geometry/dom_rect.hpp>
#include <client/dom/geometry/dom_box.hpp>

namespace client_cssom
{
  struct DefaultBoundingBox
  {
    std::optional<float> width = std::nullopt;
    std::optional<float> height = std::nullopt;
    std::optional<float> depth = std::nullopt;
  };

  inline std::ostream &operator<<(std::ostream &os, const DefaultBoundingBox &box)
  {
    os << "(";
    os << (box.width.has_value() ? std::to_string(box.width.value()) : "null") << ", ";
    os << (box.height.has_value() ? std::to_string(box.height.value()) : "null") << ", ";
    os << (box.depth.has_value() ? std::to_string(box.depth.value()) : "null");
    return os << ")";
  }

  class BoxBounding
  {
  public:
    virtual ~BoxBounding() = default;

  public:
    /**
     * The method returns a `DOMRect` object providing information about the size of an element and its position
     * relative to the viewport.
     *
     * @returns The `DOMRect` object which is the smallest rectangle which contains the entire element, including
     *          its padding and border-width.
     */
    virtual dom::geometry::DOMBox getBoundingClientRect()
    {
      return dom::geometry::DOMBox();
    }
    /**
     * The `getClientRects()` method returns a collection of `DOMRect` objects that indicate the bounding rectangles
     * for each CSS border box in a client.
     *
     * @returns a collection of `DOMRect` objects, one for each CSS border box associated with the element.
     */
    virtual std::vector<dom::geometry::DOMBox> getClientRects()
    {
      return std::vector<dom::geometry::DOMBox>();
    }

  protected:
    // The default bouding box, which will be updated to the layout system.
    //
    // Such as the `HTMLImageElement` will update the bounding box after the image is loaded or the style has changed,
    // it will compute a matched bounding box for the later layouting.
    DefaultBoundingBox defaultBoundingBox_{std::nullopt, std::nullopt, std::nullopt};
  };
}
