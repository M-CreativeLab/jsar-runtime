#pragma once

#include <memory>
#include <client/cssom/computed_style.hpp>

#include "./keyframe_effect.hpp"
#include "./timing_function.hpp"

namespace dom
{
  class Keyframe
  {
  private:
    client_cssom::ComputedStyle properties_;
    double offset_ = 0.0;
    std::unique_ptr<TimingFunction> timing_function_;
    KeyframeEffect::Composite composite_ = KeyframeEffect::CompositeReplace;
  };
}
