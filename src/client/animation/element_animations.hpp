#pragma once

#include <vector>

#include "./animation.hpp"
#include "./css/css_animations.hpp"

namespace dom
{
  class ElementAnimations final
  {
  public:
    ElementAnimations() = default;
    ElementAnimations(const ElementAnimations &) = delete;
    ElementAnimations &operator=(const ElementAnimations &) = delete;
    ~ElementAnimations() = default;

  public:
    bool isEmpty() const
    {
      return css_animations_.isEmpty() &&
             animations_.empty();
    }
    const CSSAnimations &cssAnimations() const { return css_animations_; }
    CSSAnimations &cssAnimations() { return css_animations_; }

  private:
    CSSAnimations css_animations_;
    std::vector<std::shared_ptr<Animation>> animations_;
    // TODO(yorkie): worklet animations?
  };
}
