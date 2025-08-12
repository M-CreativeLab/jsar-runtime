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
#include "./packed_splats.hpp"

namespace builtin_scene
{
  /**
   * Data structure for a single splat instance in the global rendering system.
   * Now only contains sorting-related data and a reference to texture data.
   */
  struct SplatInstanceData
  {
    uint32_t index;             // Index into the packed splat texture (changed to uint32_t)
    float depth;                // For sorting (not uploaded to GPU)
    ecs::EntityId sourceEntity; // Not uploaded to GPU

    SplatInstanceData()
        : index(0)
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
   * 
   * Uses SparkJS-compatible packed splat encoding with 3D array texture storage.
   */
  class GaussianSplatsMesh : public meshes::Splat
  {
  public:
    // Splat instance attributes layout for GPU buffer (only sorted index now)
    static constexpr size_t STRIDE = SplatInstanceData::getGPUDataSize();
    static inline std::vector<std::string> INSTANCE_ATTRIBUTES = {
      "splatIndex" // uint (sorted index into packed splat data texture array)
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
        updatePackedSplatTextureIfNeeded();
      }

      if (!needsSorting_)
      {
        return;
      }

      // Calculate depth for each splat using packed splat data
      for (auto &splat : sortedSplats_)
      {
        // Unpack position from packed data to calculate depth
        uint32_t packedIndex = splat.index;
        if (packedIndex < packedSplatData_.size())
        {
          const auto &packed = packedSplatData_[packedIndex];

          // Extract position from packed data (word1 = XY float16, word2 = Z float16 + quat data)
          // For now, use simpler unpacking for depth calculation
          // TODO: Implement proper float16 unpacking if needed for precise depth sorting

          // Use texture center position for depth calculation
          auto texCoord = packed_splat_utils::getSplatTexCoord(packedIndex);
          glm::vec3 position = extractPositionFromPacked(packed);

          glm::vec4 viewPos = viewMatrix * glm::vec4(position, 1.0f);
          splat.depth = -viewPos.z; // Depth in view space
        }
        else
        {
          splat.depth = 0.0f; // Invalid splat
        }
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
     * Update the packed splat data texture with all splat properties.
     * This uploads the packed splat data to a 3D array texture for shader access.
     */
    void updatePackedSplatTexture(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update packed splat texture if needed based on flags.
     */
    void updatePackedSplatTextureIfNeeded();

    /**
     * Get the packed splat data texture containing all compressed splat properties.
     */
    inline std::shared_ptr<client_graphics::WebGLTexture> getPackedSplatTexture() const
    {
      return packedSplatTexture_;
    }

    /**
     * Get the total number of packed splats stored in the texture.
     */
    inline size_t getTotalPackedSplats() const
    {
      return packedSplatData_.size();
    }

    /**
     * Get the splat instance buffer for attribute configuration.
     */
    inline std::shared_ptr<client_graphics::WebGLBuffer> getSplatInstanceBuffer() const
    {
      return splatInstanceBuffer_;
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
     * Extract position from packed splat data for depth calculations.
     */
    glm::vec3 extractPositionFromPacked(const PackedSplat &packed) const;

  private:
    /**
     * Rebuild the sorted splats list from all entity splats.
     * This rebuilds both the packed texture data and the sorted indices.
     */
    template <typename QueryFunc>
    void rebuildSortedSplats(QueryFunc getComponent)
    {
      packedSplatData_.clear();
      sortedSplats_.clear();

      // Collect all splats from all entities by iterating entity IDs
      uint32_t packedIndex = 0;
      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          for (const auto &splat : splats)
          {
            // Pack splat data using SparkJS-compatible encoding
            PackedSplat packed = packed_splat_utils::packSplat(
              splat.position[0], splat.position[1], splat.position[2], // position
              splat.scale[0],
              splat.scale[1],
              splat.scale[2], // scale
              splat.rotation[0],
              splat.rotation[1],
              splat.rotation[2],
              splat.rotation[3], // quaternion
              splat.color[0],
              splat.color[1],
              splat.color[2],
              splat.opacity // color + opacity
            );

            packedSplatData_.push_back(packed);

            // Add to sorted instances (only index and sorting data)
            SplatInstanceData instance;
            instance.index = packedIndex;
            instance.sourceEntity = entityId;
            sortedSplats_.push_back(instance);

            packedIndex++;
          }
        }
      }

      needsRebuild_ = false;
      needsSorting_ = true;       // After rebuilding, we need to sort the splats
      needsTextureUpdate_ = true; // After rebuilding, we need to update the texture

      // Mark the mesh as dirty so the GPU buffer gets updated
      setDirty(true);

      // Debug output
      DEBUG("GaussianSplatsMesh", "Rebuilt packed splats: %zu total splats from %zu entities", sortedSplats_.size(), splatEntities_.size());
    }

  private:
    // Vector of entity IDs that have GaussianSplattingModel3d components
    std::vector<ecs::EntityId> splatEntities_;

    // Packed splat data (SparkJS-compatible compressed format, stable during sorting)
    std::vector<PackedSplat> packedSplatData_;

    // Sorted splat indices for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // WebGL buffer for instanced splat indices
    std::shared_ptr<client_graphics::WebGLBuffer> splatInstanceBuffer_;

    // WebGL 3D array texture for packed splat data (RGBA32UI format)
    std::shared_ptr<client_graphics::WebGLTexture> packedSplatTexture_;

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