#pragma once

#include <unordered_map>
#include <list>
#include <memory>
#include <string>
#include "./atlas.hpp"

namespace builtin_scene::text::sdf
{
  /**
   * Cache key for SDF textures
   */
  struct SDFCacheKey
  {
    std::string fontFamily;
    int fontSize;
    int fontWeight;
    std::string fontStyle;
    float devicePixelRatio;
    std::string textContent;

    SDFCacheKey() = default;
    SDFCacheKey(const std::string &family, int size, int weight, const std::string &style, float dpr, const std::string &text)
        : fontFamily(family)
        , fontSize(size)
        , fontWeight(weight)
        , fontStyle(style)
        , devicePixelRatio(dpr)
        , textContent(text)
    {
    }

    bool operator==(const SDFCacheKey &other) const
    {
      return fontFamily == other.fontFamily &&
             fontSize == other.fontSize &&
             fontWeight == other.fontWeight &&
             fontStyle == other.fontStyle &&
             std::abs(devicePixelRatio - other.devicePixelRatio) < 0.001f &&
             textContent == other.textContent;
    }
  };

  /**
   * Hash function for SDFCacheKey
   */
  struct SDFCacheKeyHash
  {
    std::size_t operator()(const SDFCacheKey &key) const
    {
      std::size_t h1 = std::hash<std::string>{}(key.fontFamily);
      std::size_t h2 = std::hash<int>{}(key.fontSize);
      std::size_t h3 = std::hash<int>{}(key.fontWeight);
      std::size_t h4 = std::hash<std::string>{}(key.fontStyle);
      std::size_t h5 = std::hash<float>{}(key.devicePixelRatio);
      std::size_t h6 = std::hash<std::string>{}(key.textContent);

      // Combine hashes
      return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4) ^ (h6 << 5);
    }
  };

  /**
   * Cached SDF atlas data
   */
  struct CachedSDFAtlas
  {
    std::unique_ptr<SDFAtlas> atlas;
    std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> glyphUVs;
    size_t memorySize; // Estimated memory usage in bytes

    CachedSDFAtlas() = default;
    CachedSDFAtlas(CachedSDFAtlas &&) = default;
    CachedSDFAtlas &operator=(CachedSDFAtlas &&) = default;
  };

  /**
   * LRU cache for SDF atlases
   * 
   * This cache stores SDF atlases keyed by font parameters and text content
   * to avoid regenerating SDF textures for the same text.
   */
  class SDFCache
  {
  public:
    explicit SDFCache(size_t maxSizeBytes = 16 * 1024 * 1024); // 16MB default
    ~SDFCache() = default;

    // Non-copyable, non-movable (singleton-like behavior)
    SDFCache(const SDFCache &) = delete;
    SDFCache &operator=(const SDFCache &) = delete;
    SDFCache(SDFCache &&) = delete;
    SDFCache &operator=(SDFCache &&) = delete;

    /**
     * Get a cached atlas, or nullptr if not found
     * This moves the item to the front of the LRU list
     */
    CachedSDFAtlas *get(const SDFCacheKey &key);

    /**
     * Put an atlas in the cache
     * This may evict older entries if cache is full
     */
    void put(const SDFCacheKey &key, std::unique_ptr<SDFAtlas> atlas, std::unordered_map<uint32_t, std::unique_ptr<GlyphUV>> glyphUVs);

    /**
     * Check if key exists in cache
     */
    bool contains(const SDFCacheKey &key) const;

    /**
     * Remove specific entry from cache
     */
    void erase(const SDFCacheKey &key);

    /**
     * Clear all entries from cache
     */
    void clear();

    /**
     * Get cache statistics
     */
    struct CacheStats
    {
      size_t entryCount;
      size_t totalMemoryBytes;
      size_t maxMemoryBytes;
      double hitRate;
    };

    CacheStats getStats() const;

    /**
     * Set maximum cache size
     */
    void setMaxSize(size_t maxSizeBytes);

  private:
    using CacheList = std::list<SDFCacheKey>;
    using CacheMap = std::unordered_map<SDFCacheKey,
                                        std::pair<CachedSDFAtlas, CacheList::iterator>,
                                        SDFCacheKeyHash>;

    size_t maxSizeBytes_;
    size_t currentSizeBytes_;
    CacheList lruList_;
    CacheMap cacheMap_;

    // Statistics
    mutable size_t hitCount_;
    mutable size_t missCount_;

    // Helper methods
    void evictLRU();
    size_t estimateAtlasMemorySize(const SDFAtlas &atlas) const;
    void moveToFront(const SDFCacheKey &key);
  };

  /**
   * Global SDF cache instance
   * This provides a singleton-like interface for accessing the cache
   */
  class GlobalSDFCache
  {
  public:
    static SDFCache &getInstance();

  private:
    GlobalSDFCache() = default;
    static std::unique_ptr<SDFCache> instance_;
  };
}