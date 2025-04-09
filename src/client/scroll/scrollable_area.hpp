#pragma once

#include <glm/glm.hpp>

namespace client_scroll
{
  class ScrollableArea
  {
  public:
    ScrollableArea() = default;

  public:
    glm::vec3 scrollOrigin() const;
    float scrollWidth() const;
    float scrollHeight() const;

    glm::vec3 getScrollOffset() const;

  private:
    glm::vec3 scroll_origin_;
  };
}
