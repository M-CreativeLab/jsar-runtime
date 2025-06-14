#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <client/layout/fragment.hpp>

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
    inline void scrollBy(const glm::vec3 &offset)
    {
      scrollTo(scroll_offset_ + offset);
    }
    void scrollTo(const glm::vec3 &offset);

    void updateAfterLayout(const client_layout::Fragment &);

  private:
    glm::vec3 scroll_origin_;
    glm::vec3 scroll_offset_;
    std::optional<glm::vec3> overflow_rect_;
  };
}
