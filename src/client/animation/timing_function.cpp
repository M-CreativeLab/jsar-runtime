#include "./timing_function.hpp"

namespace dom
{
  using namespace std;

  unique_ptr<TimingFunction> TimingFunction::clone() const
  {
    if (isLinear())
    {
      return LinearTimingFunction::Clone(this);
    }
    else
    {
      throw runtime_error("Unsupported TimingFunction type for clone");
    }
  }

  string LinearTimingFunction::toString() const
  {
    return "linear";
  }

  double LinearTimingFunction::evaluate(double fraction) const
  {
    // Linear timing function is a simple linear interpolation
    return fraction;
  }

  void LinearTimingFunction::range(double *min, double *max) const
  {
    *min = 0.0;
    *max = 1.0;
  }
}
