#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <client/graphics/webgl_context.hpp>
#include "./gaussian_splats_mesh.hpp"

namespace builtin_scene
{
  // Static member for empty indices
  const Indices<uint32_t> GaussianSplatsMesh::emptyIndices_;

  GaussianSplatsMesh::GaussianSplatsMesh()
      : meshes::Splat()
      , needsRebuild_(false)
      , needsSorting_(false)
  {
  }

  void GaussianSplatsMesh::addSplatsFromEntity(ecs::EntityId entityId, const std::vector<GaussianSplat> &splats)
  {
    entitySplats_[entityId] = splats;
    needsRebuild_ = true;
    needsSorting_ = true;
  }

  void GaussianSplatsMesh::removeSplatsFromEntity(ecs::EntityId entityId)
  {
    auto it = entitySplats_.find(entityId);
    if (it != entitySplats_.end())
    {
      entitySplats_.erase(it);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::updateSplatsFromEntity(ecs::EntityId entityId, const std::vector<GaussianSplat> &splats)
  {
    entitySplats_[entityId] = splats;
    needsRebuild_ = true;
    needsSorting_ = true;
  }

  void GaussianSplatsMesh::rebuildSortedSplats()
  {
    sortedSplats_.clear();

    // Collect all splats from all entities
    for (const auto &[entityId, splats] : entitySplats_)
    {
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

    needsRebuild_ = false;
  }

  void GaussianSplatsMesh::sortSplatsByDepth(const glm::mat4 &viewMatrix)
  {
    if (needsRebuild_)
    {
      rebuildSortedSplats();
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
}
