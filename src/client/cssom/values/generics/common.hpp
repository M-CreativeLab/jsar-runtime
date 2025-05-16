#pragma once

namespace client_cssom::values::generics
{
  template <typename T>
  class NonNegative : public T
  {
    using T::T;

  public:
    NonNegative(T value) : T(value)
    {
      // TODO(yorkie): set the value to zero if the value is negative.
    }
  };
}
