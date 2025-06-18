#pragma once

#include <memory>
#include <client/animation/animation.hpp>

namespace dom
{
  class Element;
  class CSSTransition : public Animation
  {
    using Animation::Animation;

  public:
    bool isCSSTransition() const override
    {
      return true;
    }

    void clearOwningElement() override;
    // The owning element of a transition refers to the element or pseudo-element to which the `transition-property`
    // property was applied that generated the animation.
    std::shared_ptr<Element> owningElement() const override;

  private:
    std::weak_ptr<Element> owning_element_;
  };
}
