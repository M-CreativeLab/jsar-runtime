#pragma once

#include <concepts>
#include <string>
#include <glm/glm.hpp>
#include <common/utility.hpp>

namespace client_cssom
{
  namespace values
  {
    namespace computed
    {
      class Context;
    }
  }

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

    virtual ~Parse() = default;

  protected:
    virtual bool parse(const std::string &input) = 0;
  };

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
}
