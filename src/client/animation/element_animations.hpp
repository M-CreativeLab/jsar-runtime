#pragma once

#include <memory>
#include <vector>
#include <client/cssom/computed_style.hpp>

#include "./animation.hpp"
#include "./css/css_animations.hpp"

namespace dom
{
  class Element;
  class ElementAnimations final
  {
  public:
    // Create a new `ElementAnimations` object for the given target element.
    ElementAnimations(std::shared_ptr<Element> target_element);
    ElementAnimations(const ElementAnimations &) = delete;
    ElementAnimations &operator=(const ElementAnimations &) = delete;
    ~ElementAnimations() = default;

  public:
    bool isEmpty() const
    {
      return css_animations_.isEmpty() && animations_.empty();
    }
    const CSSAnimations &cssAnimations() const { return css_animations_; }
    CSSAnimations &cssAnimations() { return css_animations_; }

    // Update the element's animations to the given computed style, and returns whether the style is updated via
    // animations.
    bool updateFrameToStyle(client_cssom::ComputedStyle &);

  private:
    std::weak_ptr<Element> target_element_;
    CSSAnimations css_animations_;
    std::vector<std::shared_ptr<Animation>> animations_;
    // TODO(yorkie): worklet animations?
  };
}
