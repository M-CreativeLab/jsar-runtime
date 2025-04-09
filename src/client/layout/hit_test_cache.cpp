#include "./hit_test_cache.hpp"

namespace client_layout
{
  bool HitTestCache::lookupCachedResult(const HitTestRay &ray, HitTestResult &result)
  {
    // TODO(yorkie): support the cache for the hit test result.
    return false;
  }

  void HitTestCache::addCachedResult(const HitTestRay &ray, const HitTestResult &result)
  {
    // TODO(yorkie): support the cache for the hit test result.
  }

  void HitTestCache::clear()
  {
    items_.clear();
    update_index_ = 0;
  }
}