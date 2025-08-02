#include <client/animation/animation_effect.hpp>
#include <client/animation/animation_timeline.hpp>
#include <client/animation/animation-inl.hpp>

#include "./css_animations.hpp"

namespace dom
{
  using namespace std;

  CSSAnimations::CSSAnimations()
  {
  }

  void CSSAnimations::clearTransitions()
  {
    transitions_.clear();
  }

  size_t CSSAnimations::setTransitions(const client_cssom::ComputedStyle &new_style,
                                       const client_cssom::ComputedStyle *old_style,
                                       shared_ptr<AnimationTimeline> timeline,
                                       shared_ptr<Element> target_element)
  {
    clearTransitions();

    int len = new_style.getTransitionPropertiesCount();
    for (size_t index = 0; index < len; ++index)
    {
      auto transition_property = new_style.getTransitionProperty(index);
      if (TR_UNLIKELY(!transition_property.has_value()))
        continue;

      auto property = transition_property->property;
      std::string property_name = property.toCss();
      
      // Skip if transition duration is zero or negative
      if (transition_property->duration.seconds().value <= 0.0f)
        continue;
      
      // Skip if property is not animatable
      if (!client_cssom::ComputedStyle::IsAnimatableProperty(property_name))
        continue;

      // Get the old and new values for this property
      std::string start_value, end_value;
      if (old_style != nullptr)
      {
        auto old_it = old_style->find(property_name);
        auto new_it = new_style.find(property_name);
        
        if (old_it != old_style->end())
          start_value = old_it->second;
        if (new_it != new_style.end())
          end_value = new_it->second;
          
        // Only create transition if values are different
        if (start_value == end_value || start_value.empty() || end_value.empty())
          continue;
      }
      else
      {
        // No old style, use computed values as both start and end (no transition)
        auto new_it = new_style.find(property_name);
        if (new_it != new_style.end())
        {
          start_value = end_value = new_it->second;
        }
        continue; // Skip creating transition if no old style
      }

      auto effect = make_unique<AnimationEffect>(*transition_property);
      auto animation = make_shared<CSSTransition>(move(effect), timeline, property_name, start_value, end_value);
      
      // Set the owning element for the transition
      animation->setOwningElement(target_element);
      
      // Attach the animation to the timeline so it gets updated
      timeline->animationAttached(animation);
      
      // Start the animation
      animation->play();
      
      auto animatables = AnimatableProperties::FromTransitionProperty(property);
      auto transition_animation = make_shared<RunningTransition>(animation, animatables);
      transitions_.emplace(property_name, transition_animation);
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
