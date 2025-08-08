#include "./Cache.hpp"
#include <algorithm>

namespace builtin_scene::text::sdf
{
  SDFCache::SDFCache(size_t maxSizeBytes)
      : maxSizeBytes_(maxSizeBytes)
      , currentSizeBytes_(0)
      , hitCount_(0)
      , missCount_(0)
  {
  }

  CachedSDFAtlas *SDFCache::get(const SDFCacheKey &key)
  {
    auto it = cacheMap_.find(key);
    if (it == cacheMap_.end())
    {
      ++missCount_;
      return nullptr;
    }

    // Move to front of LRU list
    lruList_.erase(it->second.second);
    lruList_.push_front(key);
    it->second.second = lruList_.begin();

    ++hitCount_;
    return &it->second.first;
  }

  void SDFCache::put(const SDFCacheKey &key, std::unique_ptr<SDFAtlas> atlas, std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> glyphUVs)
  {
    if (!atlas)
      return;

    size_t atlasSize = estimateAtlasMemorySize(*atlas);

    // Remove existing entry if present
    erase(key);

    // Evict entries if necessary
    while (currentSizeBytes_ + atlasSize > maxSizeBytes_ && !lruList_.empty())
    {
      evictLRU();
    }

    // Don't cache if atlas is larger than max cache size
    if (atlasSize > maxSizeBytes_)
      return;

    // Add new entry
    CachedSDFAtlas cached;
    cached.atlas = std::move(atlas);
    cached.glyphUVs = std::move(glyphUVs);
    cached.memorySize = atlasSize;

    lruList_.push_front(key);
    cacheMap_[key] = std::make_pair(std::move(cached), lruList_.begin());
    currentSizeBytes_ += atlasSize;
  }

  bool SDFCache::contains(const SDFCacheKey &key) const
  {
    return cacheMap_.find(key) != cacheMap_.end();
  }

  void SDFCache::erase(const SDFCacheKey &key)
  {
    auto it = cacheMap_.find(key);
    if (it != cacheMap_.end())
    {
      currentSizeBytes_ -= it->second.first.memorySize;
      lruList_.erase(it->second.second);
      cacheMap_.erase(it);
    }
  }

  void SDFCache::clear()
  {
    cacheMap_.clear();
    lruList_.clear();
    currentSizeBytes_ = 0;
    hitCount_ = 0;
    missCount_ = 0;
  }

  SDFCache::CacheStats SDFCache::getStats() const
  {
    CacheStats stats;
    stats.entryCount = cacheMap_.size();
    stats.totalMemoryBytes = currentSizeBytes_;
    stats.maxMemoryBytes = maxSizeBytes_;

    size_t totalRequests = hitCount_ + missCount_;
    stats.hitRate = totalRequests > 0 ? static_cast<double>(hitCount_) / totalRequests : 0.0;

    return stats;
  }

  void SDFCache::setMaxSize(size_t maxSizeBytes)
  {
    maxSizeBytes_ = maxSizeBytes;

    // Evict entries if current size exceeds new max
    while (currentSizeBytes_ > maxSizeBytes_ && !lruList_.empty())
    {
      evictLRU();
    }
  }

  void SDFCache::evictLRU()
  {
    if (lruList_.empty())
      return;

    const SDFCacheKey &keyToEvict = lruList_.back();
    auto it = cacheMap_.find(keyToEvict);

    if (it != cacheMap_.end())
    {
      currentSizeBytes_ -= it->second.first.memorySize;
      cacheMap_.erase(it);
    }

    lruList_.pop_back();
  }

  size_t SDFCache::estimateAtlasMemorySize(const SDFAtlas &atlas) const
  {
    // Base atlas texture size (single channel)
    size_t textureSize = atlas.getWidth() * atlas.getHeight();

    // Add some overhead for UV data and metadata
    size_t overhead = 1024; // 1KB overhead estimate

    return textureSize + overhead;
  }

  void SDFCache::moveToFront(const SDFCacheKey &key)
  {
    auto it = cacheMap_.find(key);
    if (it != cacheMap_.end())
    {
      lruList_.erase(it->second.second);
      lruList_.push_front(key);
      it->second.second = lruList_.begin();
    }
  }

  // Global cache implementation
  std::unique_ptr<SDFCache> GlobalSDFCache::instance_;

  SDFCache &GlobalSDFCache::getInstance()
  {
    if (!instance_)
    {
      instance_ = std::make_unique<SDFCache>();
    }
    return *instance_;
  }
}