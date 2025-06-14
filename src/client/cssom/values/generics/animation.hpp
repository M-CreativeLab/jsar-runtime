#pragma once

#include <cstdint>
#include <variant>
#include <client/cssom/style_traits.hpp>
#include <client/cssom/properties.hpp>

namespace client_cssom::values::generics
{
  class GenericTransitionProperty : public ToCss
  {
  protected:
    enum Tag : uint8_t
    {
      kNonCustom,
      kCustom,
      kUnsupported,
    };
    using PropertyIdVariant = std::variant<NonCustomPropertyId, CustomPropertyId>;

  public:
    GenericTransitionProperty()
        : tag_(kCustom)
        , property_id_(CustomPropertyId("none"))
    {
    }
    GenericTransitionProperty(NonCustomPropertyId non_custom_property_id)
        : tag_(kNonCustom)
        , property_id_(non_custom_property_id)
    {
    }
    GenericTransitionProperty(CustomPropertyId custom_property_id)
        : tag_(kCustom)
        , property_id_(custom_property_id)
    {
    }

  public:
    std::string toCss() const override
    {
      if (tag_ == kNonCustom)
        return std::get<NonCustomPropertyId>(property_id_);
      else if (tag_ == kCustom)
        return std::get<CustomPropertyId>(property_id_);
      return "none"; // Unsupported case.
    }

  protected:
    Tag tag_;
    PropertyIdVariant property_id_;
  };

  class GenericTransitionBehavior
  {
  private:
    enum Tag : uint8_t
    {
      // Transitions will not be started for discrete properties, only for interpolable properties.
      kNormal,
      // Transitions will be started for discrete properties as well as interpolable properties.
      kAllowDiscrete,
    };

  public:
    static GenericTransitionBehavior Normal()
    {
      return GenericTransitionBehavior(kNormal);
    }
    static GenericTransitionBehavior AllowDiscrete()
    {
      return GenericTransitionBehavior(kAllowDiscrete);
    }

    Tag tag() const
    {
      return tag_;
    }

  private:
    GenericTransitionBehavior(Tag tag)
        : tag_(tag)
    {
    }

  private:
    Tag tag_;
  };
}
