#pragma once

#include <client/cssom/values/generics/filter.hpp>
#include <client/cssom/values/computed/filter.hpp>

namespace client_cssom::values::specified
{
#pragma once

#include <client/cssom/values/generics/filter.hpp>
#include <client/cssom/values/computed/filter.hpp>
#include <client/cssom/parsers/css_filter_parser.hpp>

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

      // Add typedef for the template parameter dependency
      using FilterFunctionType = FilterFunction;

      bool parse(const std::string &input) override
      {
        css_filter_parser::CSSFilterParser parser(input);
        auto functions = parser.parse();

        if (!parser.isValid() || functions.empty())
          return false;

        // Parse single filter function
        const auto &func = functions[0];

        switch (func.type)
        {
        case css_filter_parser::FilterFunctionType::kNone:
          tag_ = kNone;
          break;
        case css_filter_parser::FilterFunctionType::kBlur:
          tag_ = kBlur;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "px" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kBrightness:
          tag_ = kBrightness;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kContrast:
          tag_ = kContrast;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kDropShadow:
          tag_ = kDropShadow;
          raw_value_ = func.raw_value;
          break;
        case css_filter_parser::FilterFunctionType::kGrayscale:
          tag_ = kGrayscale;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kHueRotate:
          tag_ = kHueRotate;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "deg" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kInvert:
          tag_ = kInvert;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kOpacity:
          tag_ = kOpacity;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kSaturate:
          tag_ = kSaturate;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        case css_filter_parser::FilterFunctionType::kSepia:
          tag_ = kSepia;
          if (!func.values.empty())
          {
            parameters_.clear();
            parameters_.emplace_back(func.values[0], func.units.empty() ? "" : func.units[0]);
          }
          break;
        default:
          return false;
        }

        return true;
      }

      computed::FilterFunction toComputedValue(computed::Context &) const override
      {
        computed::FilterFunction result;
        result.tag_ = static_cast<computed::FilterFunction::Tag>(tag_);
        result.parameters_ = parameters_;
        result.raw_value_ = raw_value_;
        return result;
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

      // Add typedef for the template parameter dependency
      using FilterFunctionType = FilterFunction;

      bool parse(const std::string &input) override
      {
        // Handle "none" case
        if (input == "none")
        {
          is_none_ = true;
          filter_functions_.clear();
          return true;
        }

        // Use the CSS filter parser
        css_filter_parser::CSSFilterParser parser(input);
        auto parsed_functions = parser.parse();

        if (!parser.isValid())
        {
          return false;
        }

        // Convert parsed functions to FilterFunction objects
        filter_functions_.clear();
        for (const auto &parsed_func : parsed_functions)
        {
          FilterFunction filter_func;

          // Map parser function type to our internal type
          switch (parsed_func.type)
          {
          case css_filter_parser::FilterFunctionType::kBlur:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "px" : parsed_func.units[0]);
              filter_func = FilterFunction::Blur(param);
            }
            else
            {
              filter_func = FilterFunction::Blur();
            }
            break;
          case css_filter_parser::FilterFunctionType::kBrightness:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Brightness(param);
            }
            else
            {
              filter_func = FilterFunction::Brightness();
            }
            break;
          case css_filter_parser::FilterFunctionType::kContrast:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Contrast(param);
            }
            else
            {
              filter_func = FilterFunction::Contrast();
            }
            break;
          case css_filter_parser::FilterFunctionType::kDropShadow:
            filter_func = FilterFunction::DropShadow(parsed_func.raw_value);
            break;
          case css_filter_parser::FilterFunctionType::kGrayscale:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Grayscale(param);
            }
            else
            {
              filter_func = FilterFunction::Grayscale();
            }
            break;
          case css_filter_parser::FilterFunctionType::kHueRotate:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "deg" : parsed_func.units[0]);
              filter_func = FilterFunction::HueRotate(param);
            }
            else
            {
              filter_func = FilterFunction::HueRotate();
            }
            break;
          case css_filter_parser::FilterFunctionType::kInvert:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Invert(param);
            }
            else
            {
              filter_func = FilterFunction::Invert();
            }
            break;
          case css_filter_parser::FilterFunctionType::kOpacity:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Opacity(param);
            }
            else
            {
              filter_func = FilterFunction::Opacity();
            }
            break;
          case css_filter_parser::FilterFunctionType::kSaturate:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Saturate(param);
            }
            else
            {
              filter_func = FilterFunction::Saturate();
            }
            break;
          case css_filter_parser::FilterFunctionType::kSepia:
            if (!parsed_func.values.empty())
            {
              generics::FilterFunctionValue param(parsed_func.values[0],
                                                  parsed_func.units.empty() ? "" : parsed_func.units[0]);
              filter_func = FilterFunction::Sepia(param);
            }
            else
            {
              filter_func = FilterFunction::Sepia();
            }
            break;
          default:
            return false;
          }

          filter_functions_.push_back(filter_func);
        }

        is_none_ = filter_functions_.empty();
        return true;
      }

      computed::Filter toComputedValue(computed::Context &context) const override
      {
        computed::Filter result;

        if (is_none_)
        {
          result = computed::Filter::None();
        }
        else
        {
          std::vector<computed::FilterFunction> computed_functions;
          for (const auto &func : filter_functions_)
          {
            computed::FilterFunction computed_func;
            computed_func.tag_ = static_cast<computed::FilterFunction::Tag>(func.tag_);
            computed_func.parameters_ = func.parameters_;
            computed_func.raw_value_ = func.raw_value_;
            computed_functions.push_back(computed_func);
          }
          result.setFunctions(computed_functions);
        }

        return result;
      }
    };
  }