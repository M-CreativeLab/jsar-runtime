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

    // Add typedef for the template parameter dependency
    using FilterFunctionType = FilterFunction;
  };

  class Filter : public generics::GenericFilter<Filter>
  {
    using generics::GenericFilter<Filter>::GenericFilter;

  public:
    Filter()
        : generics::GenericFilter<Filter>()
    {
    }

    // Add typedef for the template parameter dependency
    using FilterFunctionType = FilterFunction;
  };
}