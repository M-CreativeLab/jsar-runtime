#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <glm/glm.hpp>

#include <client/graphics/webgl_context.hpp>
#include "./ecs.hpp"
#include "./gaussian_splatting.hpp"
#include "./meshes/splat.hpp"

namespace builtin_scene
{
  /**
   * Data structure for a single splat instance in the global rendering system.
   */
  struct SplatInstanceData
  {
    glm::vec3 position;
    glm::vec3 color;
    float opacity;
    glm::vec3 scale;
    glm::vec4 rotation;
    float depth;                // For sorting
    ecs::EntityId sourceEntity; // Which model entity this splat came from

    SplatInstanceData()
        : position(0.0f)
        , color(1.0f)
        , opacity(1.0f)
        , scale(1.0f)
        , rotation(0.0f, 0.0f, 0.0f, 1.0f)
        , depth(0.0f)
        , sourceEntity(0)
    {
    }
  };

  /**
   * Global mesh for rendering all Gaussian splats in the scene.
   * This class manages entity references for all model entities with splats,
   * handles sorting, and performs instanced rendering with the base quad geometry.
   */
  class GaussianSplatsMesh : public meshes::Splat
  {
  public:
    GaussianSplatsMesh();
    virtual ~GaussianSplatsMesh() = default;

  public:
    /**
     * Add an entity with splats to the global rendering system.
     */
    void addSplatsEntity(ecs::EntityId entityId);

    /**
     * Remove an entity from the global rendering system.
     */
    void removeSplatsEntity(ecs::EntityId entityId);

    /**
     * Update entities that have splats for rendering.
     */
    void updateSplatsEntity(ecs::EntityId entityId);

    /**
     * Sort all splats by depth for proper transparency rendering.
     * This should be called each frame before rendering.
     * Takes an ECS query function to access GaussianSplattingModel3d components.
     */
    template <typename QueryFunc>
    void sortSplatsByDepth(const glm::mat4 &viewMatrix, QueryFunc getComponent)
    {
      if (needsRebuild_)
      {
        rebuildSortedSplats(getComponent);
      }

      if (!needsSorting_)
      {
        return;
      }

      // Calculate depth for each splat in view space
      for (auto &splat : sortedSplats_)
      {
        glm::vec4 viewPos = viewMatrix * glm::vec4(splat.position, 1.0f);
        splat.depth = viewPos.z; // Depth in view space
      }

      // Sort by depth (back to front for transparency blending)
      std::sort(sortedSplats_.begin(), sortedSplats_.end(), [](const SplatInstanceData &a, const SplatInstanceData &b)
                {
                  return a.depth > b.depth; // Back to front
                });

      needsSorting_ = false;
    }

    /**
     * Get all splat instances for rendering.
     */
    inline const std::vector<SplatInstanceData> &getSplatInstances() const
    {
      return sortedSplats_;
    }

    /**
     * Get the total number of splats across all models.
     */
    inline size_t getTotalSplatCount() const
    {
      return sortedSplats_.size();
    }

    /**
     * Check if any splats need to be re-sorted.
     */
    inline bool needsSorting() const
    {
      return needsSorting_;
    }

    /**
     * Mark that splats need to be re-sorted (called when camera moves).
     */
    inline void markNeedsSorting()
    {
      needsSorting_ = true;
    }

    /**
     * Check if geometry is initialized.
     */
    inline bool isGeometryInitialized() const
    {
      return !isDirty();
    }

  private:
    /**
     * Rebuild the sorted splats list from all entity splats.
     */
    template <typename QueryFunc>
    void rebuildSortedSplats(QueryFunc getComponent)
    {
      sortedSplats_.clear();

      // Collect all splats from all entities by iterating entity IDs
      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          for (const auto &splat : splats)
          {
            SplatInstanceData instance;
            instance.position = glm::vec3(splat.position[0], splat.position[1], splat.position[2]);
            instance.color = glm::vec3(splat.color[0], splat.color[1], splat.color[2]);
            instance.opacity = splat.opacity;
            instance.scale = glm::vec3(splat.scale[0], splat.scale[1], splat.scale[2]);
            instance.rotation = glm::vec4(splat.rotation[0], splat.rotation[1], splat.rotation[2], splat.rotation[3]);
            instance.sourceEntity = entityId;

            sortedSplats_.push_back(instance);
          }
        }
      }

      needsRebuild_ = false;
    }

  private:
    // Set of entity IDs that have GaussianSplattingModel3d components
    std::unordered_set<ecs::EntityId> splatEntities_;

    // Sorted splats for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // Flags
    bool needsRebuild_;
    bool needsSorting_;

    // Empty indices to prevent normal draw call
    static const Indices<uint32_t> emptyIndices_;
  };
}