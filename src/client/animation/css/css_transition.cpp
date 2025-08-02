#include "./css_transition.hpp"
#include <client/dom/element.hpp>
#include <cstdio>  // for snprintf
#include <algorithm>  // for std::max, std::min
#include <cctype>  // for std::isdigit

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
    // Apply timing function to progress
    if (effect().getTiming().easing)
    {
      progress = effect().getTiming().easing->evaluate(progress);
    }
    
    // Handle different value types
    
    // Try to parse as numeric values with units (e.g., "10px", "5%", "1.5em")
    if (interpolateNumericWithUnit(start, end, progress))
      return last_interpolated_result_;
    
    // Try to parse as plain numeric values
    try
    {
      float start_num = std::stof(start);
      float end_num = std::stof(end);
      float result = start_num + (end_num - start_num) * progress;
      return std::to_string(result);
    }
    catch (...)
    {
      // Not numeric, continue to other types
    }
    
    // Handle color values (basic support for hex colors)
    if (start.length() == 7 && start[0] == '#' && end.length() == 7 && end[0] == '#')
    {
      return interpolateHexColor(start, end, progress);
    }
    
    // For non-interpolable values, use discrete transition at 50%
    return progress < 0.5f ? start : end;
  }

  bool CSSTransition::interpolateNumericWithUnit(const std::string &start, const std::string &end, float progress) const
  {
    // Extract numeric part and unit from strings like "10px", "5%", "1.5em"
    float start_num, end_num;
    std::string start_unit, end_unit;
    
    if (!parseValueWithUnit(start, start_num, start_unit) ||
        !parseValueWithUnit(end, end_num, end_unit))
      return false;
    
    // Units must match for interpolation
    if (start_unit != end_unit)
      return false;
    
    float result = start_num + (end_num - start_num) * progress;
    last_interpolated_result_ = std::to_string(result) + start_unit;
    return true;
  }

  bool CSSTransition::parseValueWithUnit(const std::string &value, float &number, std::string &unit) const
  {
    // Find where the numeric part ends and unit begins
    size_t unit_start = 0;
    for (size_t i = 0; i < value.length(); ++i)
    {
      char c = value[i];
      if (!std::isdigit(c) && c != '.' && c != '-' && c != '+')
      {
        unit_start = i;
        break;
      }
    }
    
    if (unit_start == 0)
      return false; // No numeric part found
    
    try
    {
      number = std::stof(value.substr(0, unit_start));
      unit = value.substr(unit_start);
      return true;
    }
    catch (...)
    {
      return false;
    }
  }

  std::string CSSTransition::interpolateHexColor(const std::string &start, const std::string &end, float progress) const
  {
    // Parse hex colors like #RRGGBB
    auto parseHex = [](const std::string &hex) -> std::tuple<int, int, int>
    {
      if (hex.length() != 7 || hex[0] != '#')
        return {0, 0, 0};
      
      int r = std::stoi(hex.substr(1, 2), nullptr, 16);
      int g = std::stoi(hex.substr(3, 2), nullptr, 16);
      int b = std::stoi(hex.substr(5, 2), nullptr, 16);
      return {r, g, b};
    };
    
    auto [start_r, start_g, start_b] = parseHex(start);
    auto [end_r, end_g, end_b] = parseHex(end);
    
    int result_r = static_cast<int>(start_r + (end_r - start_r) * progress);
    int result_g = static_cast<int>(start_g + (end_g - start_g) * progress);
    int result_b = static_cast<int>(start_b + (end_b - start_b) * progress);
    
    // Clamp to valid range
    result_r = std::max(0, std::min(255, result_r));
    result_g = std::max(0, std::min(255, result_g));
    result_b = std::max(0, std::min(255, result_b));
    
    char result[8];
    snprintf(result, sizeof(result), "#%02x%02x%02x", result_r, result_g, result_b);
    return std::string(result);
  }
}
