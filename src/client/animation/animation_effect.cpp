#include "./animation_effect.hpp"

namespace dom
{
  using namespace client_cssom;

  AnimationEffect::AnimationEffect(const ComputedStyle::TransitionProperty &transition_property)
      : timing_function_(nullptr)
  {
    timing_.delay = transition_property.delay.seconds().value;
    timing_.duration = transition_property.duration.seconds().value;
    
    // Convert the computed timing function to the animation timing function
    // For now, create a linear timing function - this should be enhanced to support
    // the actual timing function from transition_property.timing_function
    timing_function_ = LinearTimingFunction::Create({});
    timing_.easing = timing_function_.get();
  }

  AnimationEffect::ComputedTiming AnimationEffect::getComputedTiming() const
  {
    ComputedTiming computed_timing(timing_);
    
    // Calculate basic timing values
    computed_timing.activeDuration = timing_.duration * timing_.iterations;
    computed_timing.endTime = computed_timing.activeDuration + timing_.delay + timing_.endDelay;
    
    // Use the stored local time
    computed_timing.localTime = local_time_;
    
    // Calculate progress based on local time
    if (computed_timing.localTime >= timing_.delay && 
        computed_timing.localTime <= timing_.delay + timing_.duration)
    {
      float active_time = computed_timing.localTime - timing_.delay;
      computed_timing.progress = active_time / timing_.duration;
      computed_timing.currentIteration = 0; // Simple case for transitions
    }
    else if (computed_timing.localTime > timing_.delay + timing_.duration)
    {
      // Animation is finished
      computed_timing.progress = 1.0f;
      computed_timing.currentIteration = timing_.iterations - 1;
    }
    else
    {
      // Animation hasn't started yet
      computed_timing.progress = std::nullopt;
      computed_timing.currentIteration = std::nullopt;
    }
    
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
