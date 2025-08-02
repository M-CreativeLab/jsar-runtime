#pragma once

#include <memory>
#include <string>
#include <client/animation/animation.hpp>

namespace dom
{
  class Element;
  class CSSTransition : public Animation
  {
  public:
    CSSTransition(std::unique_ptr<AnimationEffect> effect,
                  std::shared_ptr<AnimationTimeline> timeline,
                  const std::string &property_name,
                  const std::string &start_value,
                  const std::string &end_value);
    
    bool isCSSTransition() const override
    {
      return true;
    }

    void clearOwningElement() override;
    // The owning element of a transition refers to the element or pseudo-element to which the `transition-property`
    // property was applied that generated the animation.
    std::shared_ptr<Element> owningElement() const override;
    void setOwningElement(std::shared_ptr<Element> element);

    // Update a specific property in the style with interpolated transition values
    bool updatePropertyToStyle(client_cssom::ComputedStyle &style, const std::string &property);

    const std::string &propertyName() const { return property_name_; }
    const std::string &startValue() const { return start_value_; }
    const std::string &endValue() const { return end_value_; }

  private:
    std::string interpolateValue(const std::string &start, const std::string &end, float progress) const;
    bool interpolateNumericWithUnit(const std::string &start, const std::string &end, float progress) const;
    bool parseValueWithUnit(const std::string &value, float &number, std::string &unit) const;
    std::string interpolateHexColor(const std::string &start, const std::string &end, float progress) const;

  private:
    std::weak_ptr<Element> owning_element_;
    std::string property_name_;
    std::string start_value_;
    std::string end_value_;
    mutable std::string last_interpolated_result_; // Cache for numeric with unit interpolation
  };
}
