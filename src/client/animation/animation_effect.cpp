#include "./animation_effect.hpp"

namespace dom
{
  AnimationEffect::ComputedTiming AnimationEffect::getComputedTiming() const
  {
    ComputedTiming computed_timing(timing_);
    // Fill in the computed timing values based on the effect's timing
    // TODO: Implement the logic to compute endTime, activeDuration, localTime, progress, and currentIteration
    return computed_timing;
  }

  AnimationEffect::Timing AnimationEffect::getTiming() const
  {
    return timing_;
  }

  void AnimationEffect::updateTimingDelay(float delay)
  {
    timing_.delay = delay;
  }

  void AnimationEffect::updateTimingDuration(float duration)
  {
    timing_.duration = duration;
  }

  void AnimationEffect::updateTimingEndDelay(float end_delay)
  {
    timing_.endDelay = end_delay;
  }

  void AnimationEffect::updateTimingIterations(size_t iterations)
  {
    timing_.iterations = iterations;
  }

  void AnimationEffect::updateTimingIterationStart(float iteration_start)
  {
    timing_.iterationStart = iteration_start;
  }

  void AnimationEffect::updateTimingDirection(Direction direction)
  {
    timing_.direction = direction;
  }

  void AnimationEffect::updateTimingFill(FillMode fill)
  {
    timing_.fill = fill;
  }

  void AnimationEffect::updateTimingEasing(std::unique_ptr<TimingFunction> easing)
  {
    timing_function_ = move(easing);
    timing_.easing = timing_function_.get();
  }
}
