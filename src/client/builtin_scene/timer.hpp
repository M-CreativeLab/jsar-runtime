#pragma once

#include <chrono>
#include "./ecs.hpp"

namespace builtin_scene
{
  /**
   * Get the current timestamp in milliseconds.
   * 
   * @returns a timestamp in milliseconds.
   */
  inline long long timestampInMilliseconds()
  {
    return std::chrono::system_clock::now().time_since_epoch().count() / 1000;
  }

  class Timer : public ecs::Resource
  {
    friend class TimerSystem;

  public:
    Timer(uint32_t interval)
        : interval_(interval),
          timestamp_(timestampInMilliseconds())
    {
    }

  public:
    /**
     * @returns the timestamp in milliseconds.
     */
    long long timestamp() { return timestamp_; }

  private:
    uint32_t interval_;
    long long timestamp_;
  };

  class TimerSystem : public ecs::System
  {
  public:
    using ecs::System::System;

  public:
    void onExecute()
    {
      auto timer = getResource<Timer>();
      if (timer)
      {
        auto nowTimestamp = timestampInMilliseconds();
        auto duration = nowTimestamp - timer->timestamp();
        if (duration >= timer->interval_)
          timer->timestamp_ = nowTimestamp;
      }
    }
  };
}
