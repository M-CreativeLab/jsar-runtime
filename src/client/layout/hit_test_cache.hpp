#pragma once

#include <unordered_map>

#include "./hit_test_ray.hpp"
#include "./hit_test_result.hpp"

namespace client_layout
{
  class HitTestCache
  {
  public:
    HitTestCache()
        : update_index_(0)
    {
    }
    HitTestCache(const HitTestCache &) = delete;
    HitTestCache &operator=(const HitTestCache &) = delete;

  public:
    bool lookupCachedResult(const HitTestRay &, HitTestResult &);
    void addCachedResult(const HitTestRay &, const HitTestResult &);

    void clear();

  private:
    unsigned update_index_;
    std::unordered_map<int, HitTestResult> items_;
  };
}
