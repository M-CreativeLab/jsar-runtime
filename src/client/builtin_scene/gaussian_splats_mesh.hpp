#pragma once

#include <vector>
#include <unordered_map>
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
   * This class manages all splats from all model entities, handles sorting,
   * and performs instanced rendering with the base quad geometry.
   */
  class GaussianSplatsMesh : public meshes::Splat
  {
  public:
    GaussianSplatsMesh();
    virtual ~GaussianSplatsMesh() = default;

  public:
    /**
     * Add splats from a model entity to the global rendering system.
     */
    void addSplatsFromEntity(ecs::EntityId entityId, const std::vector<GaussianSplat> &splats);

    /**
     * Remove splats from a model entity from the global rendering system.
     */
    void removeSplatsFromEntity(ecs::EntityId entityId);

    /**
     * Update splats from a model entity in the global rendering system.
     */
    void updateSplatsFromEntity(ecs::EntityId entityId, const std::vector<GaussianSplat> &splats);

    /**
     * Sort all splats by depth for proper transparency rendering.
     * This should be called each frame before rendering.
     */
    void sortSplatsByDepth(const glm::mat4 &viewMatrix);

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

    /**
     * Draw the splats using instanced rendering.
     */
    void drawInstanced(std::shared_ptr<client_graphics::WebGL2Context> glContext);

  private:
    /**
     * Rebuild the sorted splats list from all entity splats.
     */
    void rebuildSortedSplats();

  private:
    // Map from entity ID to its splats
    std::unordered_map<ecs::EntityId, std::vector<GaussianSplat>> entitySplats_;

    // Sorted splats for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // Flags
    bool needsRebuild_;
    bool needsSorting_;

    // Empty indices to prevent normal draw call
    static const Indices<uint32_t> emptyIndices_;
  };
}