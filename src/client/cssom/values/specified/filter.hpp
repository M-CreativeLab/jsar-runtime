#pragma once

#include <client/cssom/values/generics/filter.hpp>
#include <client/cssom/values/computed/filter.hpp>

namespace client_cssom::values::specified
{
  class FilterFunction : public generics::GenericFilterFunction<FilterFunction>,
                         public Parse,
                         public ToComputedValue<computed::FilterFunction>
  {
    friend class Parse;

  public:
    using generics::GenericFilterFunction<FilterFunction>::GenericFilterFunction;

    FilterFunction()
        : generics::GenericFilterFunction<FilterFunction>(kNone)
    {
    }

    bool parse(const std::string &input) override
    {
      if (input == "none")
        tag_ = kNone;
      else if (input.find("blur(") == 0)
        tag_ = kBlur;
      else if (input.find("brightness(") == 0)
        tag_ = kBrightness;
      else if (input.find("contrast(") == 0)
        tag_ = kContrast;
      else if (input.find("drop-shadow(") == 0)
        tag_ = kDropShadow;
      else if (input.find("grayscale(") == 0)
        tag_ = kGrayscale;
      else if (input.find("hue-rotate(") == 0)
        tag_ = kHueRotate;
      else if (input.find("invert(") == 0)
        tag_ = kInvert;
      else if (input.find("opacity(") == 0)
        tag_ = kOpacity;
      else if (input.find("saturate(") == 0)
        tag_ = kSaturate;
      else if (input.find("sepia(") == 0)
        tag_ = kSepia;
      else
        return false;
      return true;
    }

    computed::FilterFunction toComputedValue(computed::Context &) const override
    {
      if (isNone())
        return computed::FilterFunction::None();
      else if (isBlur())
        return computed::FilterFunction::Blur();
      else if (isBrightness())
        return computed::FilterFunction::Brightness();
      else if (isContrast())
        return computed::FilterFunction::Contrast();
      else if (isDropShadow())
        return computed::FilterFunction::DropShadow();
      else if (isGrayscale())
        return computed::FilterFunction::Grayscale();
      else if (isHueRotate())
        return computed::FilterFunction::HueRotate();
      else if (isInvert())
        return computed::FilterFunction::Invert();
      else if (isOpacity())
        return computed::FilterFunction::Opacity();
      else if (isSaturate())
        return computed::FilterFunction::Saturate();
      else if (isSepia())
        return computed::FilterFunction::Sepia();

      // Default to None if none match
      return computed::FilterFunction::None();
    }
  };

  class Filter : public generics::GenericFilter<Filter>,
                 public Parse,
                 public ToComputedValue<computed::Filter>
  {
    friend class Parse;

  public:
    using generics::GenericFilter<Filter>::GenericFilter;

    Filter()
        : generics::GenericFilter<Filter>()
    {
    }

    bool parse(const std::string &input) override
    {
      // Basic parsing - for now just handle "none"
      if (input == "none")
      {
        is_none_ = true;
        return true;
      }

      // TODO: Parse complex filter functions like "blur(5px) brightness(0.5)"
      // For now, accept any non-empty string as a valid filter
      if (!input.empty())
      {
        is_none_ = false;
        return true;
      }

      return false;
    }

    computed::Filter toComputedValue(computed::Context &) const override
    {
      if (is_none_)
        return computed::Filter::None();

      // TODO: Convert filter functions to computed values
      // For now, return a non-none filter
      computed::Filter result;
      result.is_none_ = false;
      return result;
    }
  };
}