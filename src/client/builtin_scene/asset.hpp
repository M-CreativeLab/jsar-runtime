#pragma once

#include <memory>
#include <unordered_map>
#include <idgen.hpp>
#include "./ecs.hpp"

namespace builtin_scene::asset
{
  typedef uint32_t AssetId;
  constexpr AssetId MAX_ASSET_ID = 10000;

  /**
   * A specialized `Resource` class for managing one kind of assets, such as meshes, textures, etc.
   *
   * @tparam T The type of the asset.
   */
  template <typename T>
  class Assets : public ecs::Resource
  {
  public:
    Assets() = default;

  public:
    /**
     * Add a new asset to the assets.
     *
     * @tparam S The type of the asset to add.
     * @param asset The asset to add.
     */
    template <typename S>
    std::shared_ptr<T> add(std::shared_ptr<S> asset)
    {
      static TrIdGenerator assetIdGen(0x1);
      AssetId assetId = assetIdGen.get();
      if (assetId >= MAX_ASSET_ID)
        throw std::runtime_error("The asset id is out of range.");

      assets_[assetId] = dynamic_pointer_cast<T>(asset);
      return asset;
    }
    /**
     * Check if the assets contain the asset of the given id.
     * 
     * @param assetId The id of the asset to check.
     * @returns `true` if the assets contain the asset of the given id, `false` otherwise.
     */
    inline bool contains(AssetId assetId)
    {
      return assets_.find(assetId) != assets_.end();
    }
    /**
     * Get the asset of the given id.
     * 
     * @param assetId The id of the asset to get.
     * @returns The asset of the given id.
     */
    inline std::shared_ptr<T> &get(AssetId assetId)
    {
      return assets_[assetId];
    }
    /**
     * Remove the asset of the given id.
     * 
     * @param assetId The id of the asset to remove.
     */
    inline void remove(AssetId assetId)
    {
      assets_.erase(assetId);
    }
    /**
     * Clear all the assets.
     */
    inline void clear()
    {
      assets_.clear();
    }
    /**
     * Check if the assets are empty.
     * 
     * @returns `true` if the assets are empty, `false` otherwise.
     */
    inline bool empty()
    {
      return assets_.empty();
    }
    /**
     * @returns The number of assets in the assets.
     */
    inline void length()
    {
      return assets_.size();
    }

  private:
    std::unordered_map<AssetId, std::shared_ptr<T>> assets_{};
  };
} // namespace builtin_scene::asset
