#pragma once

#include <optional>
#include <iostream>
#include <client/cssom/computed_style.hpp>
#include "./timing_function.hpp"

namespace dom
{
  class AnimationEffect
  {
  public:
    enum Direction
    {
      kDirectionNormal,
      kDirectionReverse,
      kDirectionAlternate,
      kDirectionAlternateReverse,
    };
    enum FillMode
    {
      kFillModeNone,
      kFillModeForwards,
      kFillModeBackwards,
      kFillModeBoth,
      kFillModeAuto,
    };

    struct Timing
    {
      float delay;
      float duration;
      float endDelay;

      size_t iterations;
      float iterationStart;

      Direction direction;
      FillMode fill;

      TimingFunction *easing = nullptr;

      Timing()
          : delay(0.0f)
          , duration(0.0f)
          , endDelay(0.0f)
          , iterations(1)
          , iterationStart(0.0f)
          , direction(kDirectionNormal)
          , fill(kFillModeNone)
      {
      }
      Timing(const Timing &other)
          : delay(other.delay)
          , duration(other.duration)
          , endDelay(other.endDelay)
          , iterations(other.iterations)
          , iterationStart(other.iterationStart)
          , direction(other.direction)
          , fill(other.fill)
      {
      }
    };

    struct ComputedTiming : public Timing
    {
      float endTime;
      float activeDuration;
      float localTime;

      std::optional<float> progress;
      std::optional<float> currentIteration;

      ComputedTiming()
          : Timing()
          , endTime(0.0f)
          , activeDuration(0.0f)
          , localTime(0.0f)
          , progress(std::nullopt)
          , currentIteration(std::nullopt)
      {
      }
      ComputedTiming(const Timing &timing)
          : Timing(timing)
          , endTime(0.0f)
          , activeDuration(0.0f)
          , localTime(0.0f)
          , progress(std::nullopt)
          , currentIteration(std::nullopt)
      {
      }
      ComputedTiming(const ComputedTiming &other)
          : Timing(other)
          , endTime(other.endTime)
          , activeDuration(other.activeDuration)
          , localTime(other.localTime)
          , progress(other.progress)
          , currentIteration(other.currentIteration)
      {
      }
    };

  public:
    AnimationEffect() = default;
    // Construct an `AnimationEffect` from a transition property.
    AnimationEffect(const client_cssom::ComputedStyle::TransitionProperty &);

    ComputedTiming getComputedTiming() const;
    Timing getTiming() const;

    void updateTimingDelay(float delay);
    void updateTimingDuration(float duration);
    void updateTimingEndDelay(float end_delay);
    void updateTimingIterations(size_t iterations);
    void updateTimingIterationStart(float iteration_start);
    void updateTimingDirection(Direction direction);
    void updateTimingFill(FillMode fill);
    void updateTimingEasing(std::unique_ptr<TimingFunction> easing);

    friend std::ostream &operator<<(std::ostream &os, const AnimationEffect &effect)
    {
      os << "AnimationEffect(" << endl
         << "  timing=" << effect.timing_.delay << endl
         << "  duration=" << effect.timing_.duration << endl
         << "  endDelay=" << effect.timing_.endDelay << endl
         << "  iterations=" << effect.timing_.iterations << endl
         << "  iterationStart=" << effect.timing_.iterationStart << endl
         << "  direction=" << effect.timing_.direction << endl
         << "  fill=" << effect.timing_.fill << endl
         << ")";
      return os;
    }

  private:
    Timing timing_;
    std::unique_ptr<TimingFunction> timing_function_ = nullptr;
  };
}
