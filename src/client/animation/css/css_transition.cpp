#include "./css_transition.hpp"
#include <client/dom/element.hpp>

namespace dom
{
  using namespace std;

  CSSTransition::CSSTransition(std::unique_ptr<AnimationEffect> effect,
                               std::shared_ptr<AnimationTimeline> timeline,
                               const std::string &property_name,
                               const std::string &start_value,
                               const std::string &end_value)
      : Animation(std::move(effect), timeline)
      , property_name_(property_name)
      , start_value_(start_value)
      , end_value_(end_value)
  {
  }

  void CSSTransition::clearOwningElement()
  {
    owning_element_.reset();
  }

  shared_ptr<Element> CSSTransition::owningElement() const
  {
    if (owning_element_.expired())
      return nullptr;
    return owning_element_.lock();
  }

  void CSSTransition::setOwningElement(std::shared_ptr<Element> element)
  {
    owning_element_ = element;
  }

  bool CSSTransition::updatePropertyToStyle(client_cssom::ComputedStyle &style, const std::string &property)
  {
    if (property != property_name_)
      return false;

    // Get the current progress of the transition
    auto computed_timing = effect().getComputedTiming();
    if (!computed_timing.progress.has_value())
      return false;

    float progress = computed_timing.progress.value();
    
    // Interpolate between start and end values
    std::string interpolated_value = interpolateValue(start_value_, end_value_, progress);
    
    // Apply the interpolated value to the style
    style[property] = interpolated_value;
    return true;
  }

  std::string CSSTransition::interpolateValue(const std::string &start, const std::string &end, float progress) const
  {
    // For now, implement basic numeric interpolation
    // This should be expanded to handle different value types (colors, lengths, etc.)
    
    // Try to parse as numeric values first
    try
    {
      float start_num = std::stof(start);
      float end_num = std::stof(end);
      float result = start_num + (end_num - start_num) * progress;
      return std::to_string(result);
    }
    catch (...)
    {
      // If not numeric, fall back to discrete transition at 50%
      return progress < 0.5f ? start : end;
    }
  }
}
