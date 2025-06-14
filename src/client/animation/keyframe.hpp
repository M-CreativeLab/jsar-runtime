#pragma once

#include <memory>
#include <client/cssom/computed_style.hpp>

#include "./keyframe_effect.hpp"
#include "./timing_function.hpp"

namespace dom
{
  class Keyframe
  {
  public:
    Keyframe();
    Keyframe(const Keyframe &other)
        : properties_(other.properties_)
        , offset_(other.offset_)
        , timing_function_(other.timing_function_ ? other.timing_function_->clone() : nullptr)
        , composite_(other.composite_)
    {
    }

  private:
    client_cssom::ComputedStyle properties_;
    double offset_ = 0.0;
    std::unique_ptr<TimingFunction> timing_function_;
    KeyframeEffect::Composite composite_ = KeyframeEffect::CompositeReplace;
  };
}
