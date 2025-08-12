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
#include "./direct_splats.hpp"

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
   * Uses direct splat storage with separate 2D textures for each property.
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

      // Calculate depth for each splat using direct splat data
      for (auto &splat : sortedSplats_)
      {
        // Get position from direct splat data to calculate depth
        uint32_t directIndex = splat.index;
        if (directIndex < directSplatData_.size())
        {
          const auto &direct = directSplatData_[directIndex];

          // Use direct position data for depth calculation
          glm::vec3 position(direct.position[0], direct.position[1], direct.position[2]);

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
     * This uploads direct splat data to separate 2D textures for shader access.
     */
    void updateSplatTextures(std::shared_ptr<client_graphics::WebGL2Context> glContext);

    /**
     * Update splat textures if needed based on flags.
     */
    void updateSplatTexturesIfNeeded();

    /**
     * Get the splat data textures containing all splat properties.
     */
    inline std::shared_ptr<client_graphics::WebGLTexture> getSplatCentersTexture() const
    {
      return splatCentersTexture_;
    }

    inline std::shared_ptr<client_graphics::WebGLTexture> getSplatColorsTexture() const
    {
      return splatColorsTexture_;
    }

    inline std::shared_ptr<client_graphics::WebGLTexture> getSplatScalesTexture() const
    {
      return splatScalesTexture_;
    }

    inline std::shared_ptr<client_graphics::WebGLTexture> getSplatQuatTexture() const
    {
      return splatQuatTexture_;
    }

    /**
     * Get the total number of direct splats stored in the textures.
     */
    inline size_t getTotalDirectSplats() const
    {
      return directSplatData_.size();
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
     * Extract position from direct splat data for depth calculations.
     */
    glm::vec3 extractPositionFromDirect(const DirectSplat &direct) const;

  private:
    /**
     * Rebuild the sorted splats list from all entity splats.
     * This rebuilds both the direct texture data and the sorted indices.
     */
    template <typename QueryFunc>
    void rebuildSortedSplats(QueryFunc getComponent)
    {
      directSplatData_.clear();
      sortedSplats_.clear();

      // Collect all splats from all entities by iterating entity IDs
      uint32_t directIndex = 0;
      for (ecs::EntityId entityId : splatEntities_)
      {
        auto *model = getComponent(entityId);
        if (model && model->isLoaded() && model->visible())
        {
          const auto &splats = model->getSplats();
          for (const auto &splat : splats)
          {
            // Convert splat data to direct format (no packing needed)
            DirectSplat direct = direct_splat_utils::convertSplat(
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

            directSplatData_.push_back(direct);

            // Add to sorted instances (only index and sorting data)
            SplatInstanceData instance;
            instance.index = directIndex;
            instance.sourceEntity = entityId;
            sortedSplats_.push_back(instance);

            directIndex++;
          }
        }
      }

      needsRebuild_ = false;
      needsSorting_ = true;       // After rebuilding, we need to sort the splats
      needsTextureUpdate_ = true; // After rebuilding, we need to update the textures

      // Mark the mesh as dirty so the GPU buffer gets updated
      setDirty(true);

      // Debug output
      DEBUG("GaussianSplatsMesh", "Rebuilt direct splats: %zu total splats from %zu entities", sortedSplats_.size(), splatEntities_.size());
    }

  private:
    // Vector of entity IDs that have GaussianSplattingModel3d components
    std::vector<ecs::EntityId> splatEntities_;

    // Direct splat data (no packing, stable during sorting)
    std::vector<DirectSplat> directSplatData_;

    // Sorted splat indices for rendering (rebuilt when entities change or camera moves)
    std::vector<SplatInstanceData> sortedSplats_;

    // WebGL buffer for instanced splat indices
    std::shared_ptr<client_graphics::WebGLBuffer> splatInstanceBuffer_;

    // WebGL 2D textures for separate splat data
    std::shared_ptr<client_graphics::WebGLTexture> splatCentersTexture_; // RGB for positions
    std::shared_ptr<client_graphics::WebGLTexture> splatColorsTexture_;  // RGBA for colors
    std::shared_ptr<client_graphics::WebGLTexture> splatScalesTexture_;  // RGB for scales
    std::shared_ptr<client_graphics::WebGLTexture> splatQuatTexture_;    // RGBA for quaternions

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