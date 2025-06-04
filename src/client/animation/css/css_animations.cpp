#include "./css_animations.hpp"

namespace dom
{
  CSSAnimations::CSSAnimations()
  {
  }

  size_t CSSAnimations::setTransitions(client_cssom::ComputedStyle &style)
  {
    // Clear existing transitions.
    transitions_.clear();

    int len = style.getTransitionPropertiesCount();
    for (size_t index = 0; index < len; ++index)
    {
      auto transition_property = style.getTransitionProperty(index);
      if (!transition_property.has_value())
        continue;

      auto property = transition_property->property;
      auto transition_animation = std::make_shared<RunningTransition>(
          std::make_shared<CSSTransition>(),
          AnimatableProperties::FromTransitionProperty(property));
      transitions_.emplace(property.toCss(), transition_animation);
    }

    return transitions_.size();
  }
}
