#include "./timing_function.hpp"

namespace dom
{
  std::string LinearTimingFunction::toString() const
  {
    return "linear";
  }

  double LinearTimingFunction::evaluate(double fraction) const
  {
    return fraction;
  }

  void LinearTimingFunction::range(double *min, double *max) const
  {
    *min = 0.0;
    *max = 1.0;
  }
}
