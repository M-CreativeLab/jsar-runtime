#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace analytics
{
  class TaggedTimepoint
  {
  public:
    TaggedTimepoint(const char *tag)
        : tag(tag)
        , timepoint(std::chrono::steady_clock::now())
    {
    }
    ~TaggedTimepoint() = default;

  public:
    std::string tag;
    std::chrono::steady_clock::time_point timepoint;
  };

  class PerformanceCounter
  {
  public:
    PerformanceCounter(const char *name)
        : name(name)
    {
#ifdef TR_ENABLE_PERF_COUNTER
      reset();
#endif
    }
    ~PerformanceCounter() = default;

  public:
    inline void reset()
    {
#ifdef TR_ENABLE_PERF_COUNTER
      startedAt = std::chrono::steady_clock::now();
#endif
      ended = false;
    }
    inline void record(const char *tag)
    {
#ifdef TR_ENABLE_PERF_COUNTER
      timepoints.push_back(tag);
#endif
    }
    inline void end()
    {
#ifdef TR_ENABLE_PERF_COUNTER
      endedAt = std::chrono::steady_clock::now();
#endif
      ended = true;
    }
    inline double duration()
    {
      if (!ended)
        return -1;

#ifdef TR_ENABLE_PERF_COUNTER
      auto time = std::chrono::duration_cast<std::chrono::microseconds>(endedAt - startedAt).count();
      return static_cast<double>(time / 1000.f);
#else
      return -1;
#endif
    }
    std::string toString()
    {
      if (!ended)
        return "";

#ifdef TR_ENABLE_PERF_COUNTER
      std::string result = name + ":";
      std::chrono::steady_clock::time_point lastTimepoint = startedAt;
      for (auto &tp : timepoints)
      {
        auto timeToTake = std::chrono::duration_cast<std::chrono::microseconds>(tp.timepoint - lastTimepoint).count();
        result += "\n  " + tp.tag + " takes " + std::to_string(static_cast<double>(timeToTake / 1000.f)) + "ms";
        lastTimepoint = tp.timepoint;
      }
      result += "\ntotal: " + std::to_string(duration()) + "ms";
      return result;
#else
      return "";
#endif
    }

  public:
    std::string name;
    std::chrono::steady_clock::time_point startedAt;
    std::chrono::steady_clock::time_point endedAt;
    std::vector<TaggedTimepoint> timepoints;
    bool ended = false;
  };
}
