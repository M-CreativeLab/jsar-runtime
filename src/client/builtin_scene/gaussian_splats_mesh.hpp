#pragma once

#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <cmath>
#include <glm/glm.hpp>

#include <client/graphics/webgl_context.hpp>
#include <common/debug.hpp>
#include "./ecs.hpp"
#include "./gaussian_splatting.hpp"
#include "./meshes/splat.hpp"

namespace builtin_scene
{
  /**
   * Data structure for splat data stored in texture.
   * This contains the actual splat properties that don't change during sorting.
   */
  struct SplatTextureData
  {
    glm::vec3 position; // 12 bytes
    glm::vec3 color;    // 12 bytes
    float opacity;      // 4 bytes
    glm::vec3 scale;    // 12 bytes
    glm::vec4 rotation; // 16 bytes
    // Total: 56 bytes per splat

    SplatTextureData()
        : position(0.0f)
        , color(1.0f)
        , opacity(1.0f)
        , scale(1.0f)
        , rotation(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }
  };

  /**
   * Data structure for a single splat instance in the global rendering system.
   * Now only contains sorting-related data and a reference to texture data.
   */
  struct SplatInstanceData
  {
    uint32_t sortedIndex;       // Index into the splat texture
    float depth;                // For sorting (not uploaded to GPU)
    ecs::EntityId sourceEntity; // Not uploaded to GPU

    SplatInstanceData()
        : sortedIndex(0)
        , depth(0.0f)
        , sourceEntity(0)
    {
    }

    // Get the size of GPU-uploadable data (only the sorted index)
    static constexpr size_t getGPUDataSize()
    {
      return sizeof(uint32_t);
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
    // Splat instance attributes layout for GPU buffer (only sorted index now)
    static constexpr size_t STRIDE = SplatInstanceData::getGPUDataSize();
    static inline std::vector<std::string> INSTANCE_ATTRIBUTES = {
      "splatSortedIndex" // uint (index into splat data texture)
    };

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
        // Update texture after rebuilding splat data
        updateSplatTextureIfNeeded();
      }

      if (!needsSorting_)
      {
        return;
      }

      // Calculate depth for each splat using texture data positions
      for (auto &splat : sortedSplats_)
      {
        const auto &textureData = splatTextureData_[splat.sortedIndex];
        glm::vec4 viewPos = viewMatrix * glm::vec4(textureData.position, 1.0f);
        splat.depth = -viewPos.z; // Depth in view space
      }

      // Sort by depth (back to front for transparency blending)
      // This reorders the sorted indices, not the texture data
      std::sort(sortedSplats_.begin(), sortedSplats_.end(), [](const SplatInstanceData &a, const SplatInstanceData &b)
                {
                  return a.depth > b.depth; // Back to front
                });

      needsSorting_ = false;

      // Mark the mesh as dirty so the GPU buffer gets updated with new sorting
      setDirty(true);
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

    /**
     * Setup the splat buffer and vertex attributes for instanced rendering.
     * Should be called when the mesh is initialized with WebGL context.
     */
    void setupSplatBuffer(std::shared_ptr<client_graphics::WebGL2Context> glContext,
                          std::shared_ptr<client_graphics::WebGLVertexArray> vao);

    /**
     * Update the splat instance buffer with current splat indices.
     * This uploads only the sorted indices to GPU for rendering.
     */
    void updateSplatBuffer(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update the splat data texture with all splat properties.
     * This uploads the splat data to a texture for shader access.
     */
    void updateSplatTexture(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update splat texture if needed based on flags.
     */
    void updateSplatTextureIfNeeded();

    /**
     * Get the splat instance buffer for attribute configuration.
     */
    inline std::shared_ptr<client_graphics::WebGLBuffer> getSplatInstanceBuffer() const
    {
      return splatInstanceBuffer_;
    }

    /**
     * Get the splat data texture containing all splat properties.
     */
    inline std::shared_ptr<client_graphics::WebGLTexture> getSplatDataTexture() const
    {
      return splatDataTexture_;
    }

    /**
     * Get the total number of splats stored in the texture.
     */
    inline size_t getTotalTextureSlots() const
    {
      return splatTextureData_.size();
    }

    /**
     * Initialize the splat buffer when the mesh is set up with WebGL context.
     */
    void onMesh3dInitialized(const Mesh3d &mesh3d,
                             std::shared_ptr<client_graphics::WebGL2Context> glContext) override;

    /**
     * Iterate over instance attributes for proper configuration.
     * This follows the same pattern as InstancedMesh for standard attribute setup.
     */
    size_t iterateInstanceAttributes(std::shared_ptr<client_graphics::WebGLProgram> program,
                                     std::function<void(const IVertexAttribute &,
                                                        int,
                                                        size_t,
                                                        size_t)> callback) const;

  private:
    /**
     * Rebuild the sorted splats list from all entity splats.
     * This rebuilds both the texture data and the sorted indices.
     */
    template <typename QueryFunc>
    void rebuildSortedSplats(QueryFunc getComponent)
    {
      splatTextureData_.clear();
      sortedSplats_.clear();

      // Collect all splats from all entities by iterating entity IDs
      uint32_t textureIndex = 0;
      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          for (const auto &splat : splats)
          {
            // Add to texture data (stable data that doesn't change during sorting)
            SplatTextureData textureData;
            textureData.position = glm::vec3(splat.position[0], splat.position[1], splat.position[2]);
            textureData.color = glm::vec3(splat.color[0], splat.color[1], splat.color[2]);
            textureData.opacity = splat.opacity;
            textureData.scale = glm::vec3(splat.scale[0], splat.scale[1], splat.scale[2]);
            textureData.rotation = glm::vec4(splat.rotation[0], splat.rotation[1], splat.rotation[2], splat.rotation[3]);
            splatTextureData_.push_back(textureData);

            // Add to sorted instances (only index and sorting data)
            SplatInstanceData instance;
            instance.sortedIndex = textureIndex;
            instance.sourceEntity = entityId;
            sortedSplats_.push_back(instance);

            textureIndex++;
          }
        }
      }

      needsRebuild_ = false;
      needsSorting_ = true;       // After rebuilding, we need to sort the splats
      needsTextureUpdate_ = true; // After rebuilding, we need to update the texture

      // Mark the mesh as dirty so the GPU buffer gets updated
      setDirty(true);

      // Debug output
      DEBUG("GaussianSplatsMesh", "Rebuilt splats: %zu total splats from %zu entities", sortedSplats_.size(), splatEntities_.size());
    }

  private:
    // Vector of entity IDs that have GaussianSplattingModel3d components
    std::vector<ecs::EntityId> splatEntities_;

    // Splat texture data (stable, doesn't change during sorting)
    std::vector<SplatTextureData> splatTextureData_;

    // Sorted splat indices for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // WebGL buffer for instanced splat indices
    std::shared_ptr<client_graphics::WebGLBuffer> splatInstanceBuffer_;

    // WebGL texture for splat data (position, color, opacity, scale, rotation)
    std::shared_ptr<client_graphics::WebGLTexture> splatDataTexture_;

    // WebGL context reference (needed for iterateInstanceAttributes)
    std::weak_ptr<client_graphics::WebGL2Context> glContext_;

    // Flags
    bool needsRebuild_;
    bool needsSorting_;
    bool bufferInitialized_;
    bool textureInitialized_;
    bool needsTextureUpdate_;

    // Empty indices to prevent normal draw call
    static const Indices<uint32_t> emptyIndices_;
  };
}