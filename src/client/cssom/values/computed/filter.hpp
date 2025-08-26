#pragma once

#include <client/cssom/values/generics/filter.hpp>

namespace client_cssom::values::specified
{
  class Filter;
}

namespace client_cssom::values::computed
{
  class FilterFunction : public generics::GenericFilterFunction<FilterFunction>
  {
    using generics::GenericFilterFunction<FilterFunction>::GenericFilterFunction;

  public:
    FilterFunction()
        : generics::GenericFilterFunction<FilterFunction>(kNone)
    {
    }

    // TODO: Add value storage and conversion methods when needed
    // For now, just support the basic function types
  };

  class Filter : public generics::GenericFilter<Filter>
  {
    using generics::GenericFilter<Filter>::GenericFilter;

  public:
    Filter()
        : generics::GenericFilter<Filter>()
    {
    }

    // TODO: Add filter function list and operations when needed
    // For now, just support "none" and basic structure
  };
}