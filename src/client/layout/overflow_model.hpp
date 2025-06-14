#pragma once

#include <optional>
#include <glm/glm.hpp>
#include "./geometry/rect.hpp"

namespace client_layout
{
  class BoxScrollableOverflowModel
  {
  public:
    explicit BoxScrollableOverflowModel(geometry::Rect<float> overflowRect)
        : scrollable_overflow_(overflowRect)
    {
    }
    BoxScrollableOverflowModel(const BoxScrollableOverflowModel &) = delete;
    BoxScrollableOverflowModel &operator=(const BoxScrollableOverflowModel &) = delete;

  public:
    const geometry::Rect<float> &scrollableOverflowRect() const
    {
      return scrollable_overflow_;
    }

  private:
    geometry::Rect<float> scrollable_overflow_;
  };

  class BoxVisualOverflowModel
  {
  public:
    explicit BoxVisualOverflowModel(const geometry::Rect<float> &selfVisualOverflowRect)
        : self_visual_overflow_(selfVisualOverflowRect)
    {
    }
    BoxVisualOverflowModel(const BoxVisualOverflowModel &) = delete;
    BoxVisualOverflowModel &operator=(const BoxVisualOverflowModel &) = delete;

  public:
    void setSelfVisualOverflow(const geometry::Rect<float> &rect)
    {
      self_visual_overflow_ = rect;
    }

    const geometry::Rect<float> &selfVisualOverflowRect() const
    {
      return self_visual_overflow_;
    }
    void addSelfVisualOverflow(const geometry::Rect<float> &rect)
    {
    }

    const geometry::Rect<float> &contentsVisualOverflowRect() const
    {
      return contents_visual_overflow_;
    }
    void addContentsVisualOverflow(const geometry::Rect<float> &rect)
    {
    }

    void move(float dx, float dy)
    {
      glm::vec2 offset(dx, dy);
      self_visual_overflow_.move(offset);
      contents_visual_overflow_.move(offset);
    }

  private:
    geometry::Rect<float> self_visual_overflow_;
    geometry::Rect<float> contents_visual_overflow_;
  };

  struct BoxOverflowModel
  {
    std::optional<BoxScrollableOverflowModel> scrollableOverflow;
    std::optional<BoxVisualOverflowModel> visualOverflow;

    struct PreviousOverflowData
    {
      geometry::Rect<float> previousScrollableOverflowRect;
      geometry::Rect<float> previousVisualOverflowRect;
      geometry::Rect<float> previousSelfVisualOverflowRect;
    };
    std::optional<PreviousOverflowData> previousOverflowData;
  };
}
