#pragma once

#include <client/cssom/values/generics/filter.hpp>

namespace endor
{
  namespace client_cssom::values
  {
    namespace specified
    {
      class FilterFunction;
      class Filter;
    }

    namespace computed
    {
      class FilterFunction : public generics::GenericFilterFunction<FilterFunction>
      {
        friend class specified::FilterFunction;
        friend class specified::Filter;
        using generics::GenericFilterFunction<FilterFunction>::GenericFilterFunction;

      public:
        FilterFunction()
            : generics::GenericFilterFunction<FilterFunction>(kNone)
        {
        }
      };

      class Filter : public generics::GenericFilter<Filter, FilterFunction>
      {
        using generics::GenericFilter<Filter, FilterFunction>::GenericFilter;
      };
    }
  }
} // namespace endor
