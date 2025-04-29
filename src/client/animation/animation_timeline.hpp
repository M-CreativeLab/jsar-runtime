#pragma once

namespace dom
{
  class AnimationTimeline
  {
  public:
    long currentTime() const { return current_time_; }

  protected:
    long current_time_ = 0;
  };
}
