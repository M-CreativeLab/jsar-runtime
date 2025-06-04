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

  bool CSSAnimations::updateFrameToStyle(client_cssom::ComputedStyle &style)
  {
    bool updated = false;

    // Update the CSS animations.
    for (const auto &animation : running_animations_)
    {
      if (animation == nullptr)
        continue;
      animation->updateFrameToStyle(style);
    }

    // Update the CSS transitions.
    for (auto &transition : transitions_)
    {
      auto running_transition = transition.second;
      if (TR_UNLIKELY(running_transition == nullptr))
        continue;
      if (running_transition->updateFrameToStyle(style) > 0)
        updated = true;
    }
    return updated;
  }
}
