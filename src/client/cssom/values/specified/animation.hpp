#pragma once

#include <vector>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/values/generics/animation.hpp>
#include <client/cssom/values/computed/animation.hpp>

namespace client_cssom::values::specified
{
  class TransitionProperty : public generics::GenericTransitionProperty,
                             public Parse,
                             public ToComputedValue<computed::TransitionProperty>
  {
    friend class Parse;
    using generics::GenericTransitionProperty::GenericTransitionProperty;

  private:
    bool parse(const std::string &input_str) override
    {
      auto non_custom_property_id = NonCustomPropertyId::Parse(input_str);
      if (non_custom_property_id.has_value())
      {
        tag_ = kNonCustom;
        property_id_ = *non_custom_property_id;
      }
      else
      {
        tag_ = kCustom;
        property_id_ = CustomPropertyId(input_str);
      }
      return true;
    }

  public:
    computed::TransitionProperty toComputedValue(computed::Context &context) const override
    {
      if (tag_ == kNonCustom)
        return computed::TransitionProperty(std::get<NonCustomPropertyId>(property_id_));
      else if (tag_ == kCustom)
        return computed::TransitionProperty(std::get<CustomPropertyId>(property_id_));
      return computed::TransitionProperty(); // Unsupported case.
    }
  };

  class TransitionPropertySet : std::vector<TransitionProperty>,
                                public Parse,
                                public ToCss,
                                public ToComputedValue<std::vector<computed::TransitionProperty>>
  {
    friend class Parse;

  public:
    TransitionPropertySet() = default;
    TransitionPropertySet(const std::vector<TransitionProperty> &properties)
        : std::vector<TransitionProperty>(properties)
    {
    }

  private:
    bool parse(const std::string &input_str) override
    {
      // Clear the existing properties.
      clear();

      size_t start = 0;
      size_t end = input_str.find(',');

      while (true)
      {
        std::string prop_str = input_str.substr(start, end - start);

        // Trim leading and trailing whitespace of the `prop`
        prop_str.erase(prop_str.begin(), std::find_if(prop_str.begin(), prop_str.end(), [](int ch)
                                                      { return !std::isspace(ch); }));
        prop_str.erase(std::find_if(prop_str.rbegin(), prop_str.rend(), [](int ch)
                                    { return !std::isspace(ch); })
                         .base(),
                       prop_str.end());

        // Try inserting the property.
        if (!prop_str.empty())
          push_back(Parse::ParseSingleValue<TransitionProperty>(prop_str));

        // Break if we reach the end of the string
        if (end == std::string::npos)
        {
          break;
        }

        // Go next
        start = end + 1;
        end = input_str.find(',', start);
      }
      return true;
    }

  public:
    std::string toCss() const override
    {
      std::string result;
      for (const auto &property : *this)
      {
        if (!result.empty())
          result += ", ";
        result += property.toCss();
      }
      return result;
    }
    std::vector<computed::TransitionProperty> toComputedValue(computed::Context &context) const override
    {
      std::vector<computed::TransitionProperty> computed_properties;
      for (const auto &property : *this)
        computed_properties.push_back(property.toComputedValue(context));
      return computed_properties;
    }
  };
}
