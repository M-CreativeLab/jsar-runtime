#include <algorithm>
#include <functional>
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

  void GaussianSplatsMesh::addSplatsEntity(ecs::EntityId entityId)
  {
    splatEntities_.insert(entityId);
    needsRebuild_ = true;
    needsSorting_ = true;
  }

  void GaussianSplatsMesh::removeSplatsEntity(ecs::EntityId entityId)
  {
    auto it = splatEntities_.find(entityId);
    if (it != splatEntities_.end())
    {
      splatEntities_.erase(it);
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }

  void GaussianSplatsMesh::updateSplatsEntity(ecs::EntityId entityId)
  {
    if (splatEntities_.count(entityId) > 0)
    {
      needsRebuild_ = true;
      needsSorting_ = true;
    }
  }
}
