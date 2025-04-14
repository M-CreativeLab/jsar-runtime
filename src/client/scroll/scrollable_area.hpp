#pragma once

#include <glm/glm.hpp>

namespace client_scroll
{
  class ScrollableArea
  {
  public:
    ScrollableArea();

  public:
    glm::vec3 scrollOrigin() const;
    float scrollWidth() const;
    float scrollHeight() const;

    glm::vec3 getScrollOffset() const;
    void scrollBy(const glm::vec3 &offset);
    void scrollTo(const glm::vec3 &offset);

  private:
    glm::vec3 scroll_origin_;
    glm::vec3 scroll_offset_;
  };
}
