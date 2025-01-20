#pragma once

#include <client/dom/geometry/dom_rect.hpp>

namespace client_cssom
{
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
    virtual dom::geometry::DOMRect getBoundingClientRect()
    {
      return dom::geometry::DOMRect();
    }
    /**
     * The `getClientRects()` method returns a collection of `DOMRect` objects that indicate the bounding rectangles 
     * for each CSS border box in a client.
     * 
     * @returns a collection of `DOMRect` objects, one for each CSS border box associated with the element.
     */
    virtual std::vector<dom::geometry::DOMRect> getClientRects()
    {
      return std::vector<dom::geometry::DOMRect>();
    }
  };
}
