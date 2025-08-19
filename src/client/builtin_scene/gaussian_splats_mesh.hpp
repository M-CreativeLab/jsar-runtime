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
#include "./compressed_splats.hpp"

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
   * Uses compressed splat storage with texture2D for optimal GPU performance.
   * Each splat is stored in 1 texel (4 floats) instead of 2 texels (8 floats).
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
        updateSplatTexturesIfNeeded();
      }

      if (!needsSorting_)
      {
        return;
      }

      // Calculate depth for each splat using compressed splat data
      for (auto &splat : sortedSplats_)
      {
        // Get position from compressed splat data to calculate depth
        uint32_t compressedIndex = splat.index;
        if (compressedIndex < compressedSplatData_.size())
        {
          const auto &compressed = compressedSplatData_[compressedIndex];

          // Use compressed position data for depth calculation (word0 and word1 contain half-float position)
          auto pos = compressed_splat_utils::decompressPositionHalf(compressed.word[0], compressed.word[1]);
          glm::vec3 position(pos[0], pos[1], pos[2]);

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
     * Update the splat data textures with all splat properties.
     * This uploads compressed splat data to a texture2DArray for shader access.
     */
    void updateSplatTextures(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update splat textures if needed based on flags.
     */
    void updateSplatTexturesIfNeeded();

    /**
     * Get the compressed splat texture array containing all splat properties.
     */
    inline std::shared_ptr<client_graphics::WebGLTexture> getCompressedSplatsTexture() const
    {
      return compressedSplatsTexture_;
    }

    /**
     * Get the total number of compressed splats stored in the texture array.
     */
    inline size_t getTotalCompressedSplats() const
    {
      return compressedSplatData_.size();
    }

    /**
     * Get the normalization parameters for position and scale compression.
     */
    inline const SplatNormalizationParams &getNormalizationParams() const
    {
      return normalizationParams_;
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
    void onMesh3dInitialized(std::shared_ptr<Mesh3d> mesh3d,
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
     * This rebuilds both the compressed texture data and the sorted indices.
     */
    template <typename QueryFunc>
    void rebuildSortedSplats(QueryFunc getComponent)
    {
      compressedSplatData_.clear();
      sortedSplats_.clear();

      // Collect all splats from all entities and compute bounds for normalization
      std::vector<GaussianSplat> allSplats;

      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          allSplats.insert(allSplats.end(), splats.begin(), splats.end());
        }
      }

      if (allSplats.empty())
      {
        needsRebuild_ = false;
        needsSorting_ = false;
        needsTextureUpdate_ = true;
        setDirty(true);
        return;
      }

      // Compute scale bounds for log compression (no position bounds needed for half-floats)
      float scaleMin[3] = {std::log2(std::max(0.001f, allSplats[0].scale[0])),
                           std::log2(std::max(0.001f, allSplats[0].scale[1])),
                           std::log2(std::max(0.001f, allSplats[0].scale[2]))};
      float scaleMax[3] = {scaleMin[0], scaleMin[1], scaleMin[2]};

      for (const auto &splat : allSplats)
      {
        // Update scale bounds (log2 space) - no position bounds needed for half-floats
        for (int i = 0; i < 3; i++)
        {
          float logScale = std::log2(std::max(0.001f, splat.scale[i]));
          scaleMin[i] = std::min(scaleMin[i], logScale);
          scaleMax[i] = std::max(scaleMax[i], logScale);
        }
      }

      // Store normalization parameters (only scale bounds needed)
      for (int i = 0; i < 3; i++)
      {
        normalizationParams_.scaleMin[i] = scaleMin[i];
        normalizationParams_.scaleMax[i] = scaleMax[i];
      }

      // Now convert all splats to compressed format
      uint32_t compressedIndex = 0;
      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          for (const auto &splat : splats)
          {
            // Convert splat data to compressed format (1 texel per splat)
            CompressedSplat compressed = compressed_splat_utils::convertSplat(
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
              splat.opacity,       // color + opacity
              normalizationParams_ // normalization parameters
            );

            compressedSplatData_.push_back(compressed);

            // Add to sorted instances (only index and sorting data)
            SplatInstanceData instance;
            instance.index = compressedIndex;
            instance.sourceEntity = entityId;
            sortedSplats_.push_back(instance);

            compressedIndex++;
          }
        }
      }

      needsRebuild_ = false;
      needsSorting_ = true;       // After rebuilding, we need to sort the splats
      needsTextureUpdate_ = true; // After rebuilding, we need to update the textures

      // Mark the mesh as dirty so the GPU buffer gets updated
      setDirty(true);

      // Debug output
      DEBUG("GaussianSplatsMesh", "Rebuilt compressed splats: %zu total splats from %zu entities", sortedSplats_.size(), splatEntities_.size());
    }

  private:
    // Vector of entity IDs that have GaussianSplattingModel3d components
    std::vector<ecs::EntityId> splatEntities_;

    // Compressed splat data (1 texel per splat: 4 words, stable during sorting)
    std::vector<CompressedSplat> compressedSplatData_;

    // Sorted splat indices for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // WebGL buffer for instanced splat indices
    std::shared_ptr<client_graphics::WebGLBuffer> splatInstanceBuffer_;

    // WebGL texture2D for compressed splat data (1 texel per splat)
    std::shared_ptr<client_graphics::WebGLTexture> compressedSplatsTexture_;

    // Normalization parameters for position and scale compression
    SplatNormalizationParams normalizationParams_;

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