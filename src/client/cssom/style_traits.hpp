#pragma once

#include <concepts>
#include <string>
#include <glm/glm.hpp>
#include <common/utility.hpp>

namespace client_cssom
{
  // Forward declaration of the `values::computed::Context` class.
  namespace values
  {
    namespace computed
    {
      class Context;
    }
  }

  class ToCss
  {
  public:
    virtual ~ToCss() = default;
    virtual std::string toCss() const = 0;
  };

  template <typename ComputedValue>
  class ToComputedValue
  {
  public:
    virtual ~ToComputedValue() = default;
    virtual ComputedValue toComputedValue(values::computed::Context &) const = 0;
  };

  template <typename LayoutValue>
  class ToLayoutValue
  {
  public:
    virtual ~ToLayoutValue() = default;
    virtual LayoutValue toLayoutValue() const = 0;
  };

  template <typename T>
  class SpecifiedValuesArray : public std::vector<T>
  {
    using std::vector<T>::vector;

  public:
    // Convert the array to a vector of computed values.
    template <typename ComputedType>
    std::vector<ComputedType> toComputedValues(values::computed::Context &context) const
    {
      std::vector<ComputedType> computed_result;
      for (const auto &value : *this)
        computed_result.push_back(value.toComputedValue(context));
      return computed_result;
    }
  };

  class Parse
  {
  public:
    // Parse a string to a single value of type T which is derived from Parse.
    template <typename T>
      requires transmute::common::derived_from<T, Parse>
    static T ParseSingleValue(const std::string &input)
    {
      T value;
      if (!input.empty() && value.parse(input))
        return value;
      return T();
    }

    // Parse a string(such as "1s,2s") to an array of type T which is derived from Parse.
    template <typename T>
      requires transmute::common::derived_from<T, Parse>
    static SpecifiedValuesArray<T> ParseValuesArray(const std::string &input)
    {
      SpecifiedValuesArray<T> values;
      size_t start = 0;
      size_t end = 0;

      while ((end = input.find(',', start)) != std::string::npos)
      {
        std::string token = input.substr(start, end - start);
        // Trim whitespace from the token
        token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](int ch)
                                                { return !std::isspace(ch); }));
        token.erase(std::find_if(token.rbegin(), token.rend(), [](int ch)
                                 { return !std::isspace(ch); })
                        .base(),
                    token.end());

        values.push_back(ParseSingleValue<T>(token));
        start = end + 1;
      }

      // Handle the last token
      if (start < input.size())
        values.push_back(ParseSingleValue<T>(input.substr(start)));
      return values;
    }

    virtual ~Parse() = default;

  protected:
    // Implement this method to parse the given input string into the specific value type.
    virtual bool parse(const std::string &input) = 0;
  };
}
