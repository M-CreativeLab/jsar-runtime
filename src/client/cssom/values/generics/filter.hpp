#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <client/cssom/style_traits.hpp>

namespace endor
{
  namespace client_cssom::values::generics
  {
    // Base class for filter function parameters
    struct FilterFunctionValue
    {
      double value = 0.0;
      std::string unit;

      FilterFunctionValue() = default;
      FilterFunctionValue(double v, const std::string &u = "")
          : value(v)
          , unit(u)
      {
      }

      std::string toString() const
      {
        std::ostringstream oss;
        oss << std::fixed;

        if (unit.empty())
        {
          oss << std::setprecision(1) << value;
        }
        else
        {
          if (unit == "%")
            oss << std::setprecision(0) << (value * 100);
          else if (unit == "deg" || unit == "px")
            oss << std::setprecision(0) << value;
          else
            oss << std::setprecision(2) << value;
          oss << unit;
        }
        return oss.str();
      }
    };

    template <typename T>
    class GenericFilterFunction : public ToCss
    {
    protected:
      enum Tag : uint8_t
      {
        kNone = 0,
        kBlur,
        kBrightness,
        kContrast,
        kDropShadow,
        kGrayscale,
        kHueRotate,
        kInvert,
        kOpacity,
        kSaturate,
        kSepia,
      };

    public:
      static T None()
      {
        return T(kNone);
      }
      static T Blur(const FilterFunctionValue &length = FilterFunctionValue(0.0, "px"))
      {
        T result(kBlur);
        result.parameters_.push_back(length);
        return result;
      }
      static T Brightness(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kBrightness);
        result.parameters_.push_back(value);
        return result;
      }
      static T Contrast(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kContrast);
        result.parameters_.push_back(value);
        return result;
      }
      static T DropShadow(const std::string &shadow_value = "")
      {
        T result(kDropShadow);
        result.raw_value_ = shadow_value;
        return result;
      }
      static T Grayscale(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kGrayscale);
        result.parameters_.push_back(value);
        return result;
      }
      static T HueRotate(const FilterFunctionValue &angle = FilterFunctionValue(0.0, "deg"))
      {
        T result(kHueRotate);
        result.parameters_.push_back(angle);
        return result;
      }
      static T Invert(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kInvert);
        result.parameters_.push_back(value);
        return result;
      }
      static T Opacity(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kOpacity);
        result.parameters_.push_back(value);
        return result;
      }
      static T Saturate(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kSaturate);
        result.parameters_.push_back(value);
        return result;
      }
      static T Sepia(const FilterFunctionValue &value = FilterFunctionValue(1.0))
      {
        T result(kSepia);
        result.parameters_.push_back(value);
        return result;
      }

    protected:
      GenericFilterFunction(Tag tag)
          : tag_(tag)
      {
      }

    public:
      GenericFilterFunction()
          : tag_(kNone)
      {
      }
      inline bool isNone() const
      {
        return tag_ == kNone;
      }
      inline bool isBlur() const
      {
        return tag_ == kBlur;
      }
      inline bool isBrightness() const
      {
        return tag_ == kBrightness;
      }
      inline bool isContrast() const
      {
        return tag_ == kContrast;
      }
      inline bool isDropShadow() const
      {
        return tag_ == kDropShadow;
      }
      inline bool isGrayscale() const
      {
        return tag_ == kGrayscale;
      }
      inline bool isHueRotate() const
      {
        return tag_ == kHueRotate;
      }
      inline bool isInvert() const
      {
        return tag_ == kInvert;
      }
      inline bool isOpacity() const
      {
        return tag_ == kOpacity;
      }
      inline bool isSaturate() const
      {
        return tag_ == kSaturate;
      }
      inline bool isSepia() const
      {
        return tag_ == kSepia;
      }

      // Access parameters
      const std::vector<FilterFunctionValue> &getParameters() const
      {
        return parameters_;
      }

      const std::string &getRawValue() const
      {
        return raw_value_;
      }

      std::string toCss() const override
      {
        std::string result;
        switch (tag_)
        {
        case kNone:
          return "none";
        case kBlur:
          result = "blur(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kBrightness:
          result = "brightness(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kContrast:
          result = "contrast(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kDropShadow:
          result = "drop-shadow(";
          result += raw_value_;
          result += ")";
          return result;
        case kGrayscale:
          result = "grayscale(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kHueRotate:
          result = "hue-rotate(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kInvert:
          result = "invert(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kOpacity:
          result = "opacity(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kSaturate:
          result = "saturate(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        case kSepia:
          result = "sepia(";
          if (!parameters_.empty())
            result += parameters_[0].toString();
          result += ")";
          return result;
        default:
          return "none";
        }
      }

    protected:
      Tag tag_;
      std::vector<FilterFunctionValue> parameters_;
      std::string raw_value_; // For complex values like drop-shadow
    };

    template <typename T, typename F>
    class GenericFilter : public ToCss
    {
    public:
      GenericFilter()
          : is_none_(true)
      {
      }

      static T None()
      {
        T filter;
        filter.is_none_ = true;
        return filter;
      }

      inline bool isNone() const
      {
        return is_none_;
      }

      // Add filter functions to the list
      void addFunction(const F &func)
      {
        if (is_none_)
        {
          is_none_ = false;
          filter_functions_.clear();
        }
        filter_functions_.push_back(func);
      }

      // Get filter functions
      const std::vector<F> &getFunctions() const
      {
        return filter_functions_;
      }

      // Set filter functions
      void setFunctions(const std::vector<F> &functions)
      {
        filter_functions_ = functions;
        is_none_ = functions.empty();
      }

      std::string toCss() const override
      {
        if (is_none_ || filter_functions_.empty())
          return "none";

        std::string result;
        for (size_t i = 0; i < filter_functions_.size(); ++i)
        {
          if (i > 0)
            result += " ";
          result += filter_functions_[i].toCss();
        }
        return result;
      }

    protected:
      bool is_none_;
      std::vector<F> filter_functions_;
    };
  }
} // namespace endor
