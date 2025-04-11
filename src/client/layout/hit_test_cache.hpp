#pragma once

#include <array>

#include "./hit_test_ray.hpp"
#include "./hit_test_result.hpp"

namespace client_layout
{
  struct HitTestCacheItem
  {
    HitTestRay ray;
    HitTestResult result;

    void cacheValues(const HitTestCacheItem &other);
  };

  class HitTestCache
  {
  private:
    static constexpr size_t kMaxCacheSize = 2;

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
    std::vector<HitTestCacheItem> items_;
  };
}
