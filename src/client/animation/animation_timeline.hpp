#pragma once

#include <optional>
#include <chrono>

namespace dom
{
  class AnimationTimeline
  {
  public:
    inline bool isActive() const
    {
      return current_time_ != std::nullopt;
    }
    std::optional<long long> currentTime() const
    {
      if (!isActive())
        return std::nullopt;
      return std::chrono::duration_cast<std::chrono::milliseconds>(current_time_->time_since_epoch()).count();
    }

  protected:
    void updateCurrentTime()
    {
      current_time_ = std::chrono::high_resolution_clock::now();
    }

  protected:
    std::optional<std::chrono::time_point<std::chrono::high_resolution_clock>> current_time_;
  };
}
