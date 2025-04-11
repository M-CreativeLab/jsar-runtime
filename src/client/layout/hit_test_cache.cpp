#include "./hit_test_cache.hpp"

namespace client_layout
{
  void HitTestCacheItem::cacheValues(const HitTestCacheItem &other)
  {
    *this = other;
    // result.cacheValues(other.result);
  }

  bool HitTestCache::lookupCachedResult(const HitTestRay &ray, HitTestResult &result)
  {
    if (result.getHitTestRequest().avoidCache())
      return false;

    for (const auto &cachedItem : items_)
    {
      if (cachedItem.ray == ray)
      {
        result = cachedItem.result;
        return true;
      }
    }
    return false;
  }

  void HitTestCache::addCachedResult(const HitTestRay &ray, const HitTestResult &result)
  {
    if (items_.size() < kMaxCacheSize)
      items_.resize(update_index_ + 1);

    HitTestCacheItem item;
    item.ray = ray;
    item.result = result;
    items_[update_index_].cacheValues(item);

    update_index_ += 1;
    if (update_index_ >= kMaxCacheSize)
      update_index_ = 0;
  }

  void HitTestCache::clear()
  {
    items_.clear();
    update_index_ = 0;
  }
}
