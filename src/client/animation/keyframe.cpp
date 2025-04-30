#include "./keyframe.hpp"

namespace dom
{
  Keyframe::Keyframe()
      : offset_(0.0),
        timing_function_(nullptr),
        composite_(KeyframeEffect::CompositeReplace)
  {
  }
}
