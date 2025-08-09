#include <memory>

#include "./gaussian_splatting.hpp"
#include "./gaussian_splats_mesh.hpp"
#include "./hierarchy.hpp"
#include "./materials.hpp"
#include "./meshes.hpp"
#include "./transform.hpp"
#include "./xr.hpp"

namespace builtin_scene::gaussian_splatting
{
  using namespace std;
  using namespace ecs;

  void GaussianSplattingInitSystem::onExecute()
  {
    auto gaussianSplattingCtx = getResource<GaussianSplattingContext>();
    auto meshes = getResource<Meshes>();
    auto materials = getResource<Materials>();
    assert(gaussianSplattingCtx != nullptr &&
           meshes != nullptr &&
           materials != nullptr);

    // Create the global GaussianSplatsMesh entity for rendering all Gaussian splats
    gaussianSplattingCtx->globalSplatsMeshEntity_ = spawn(
      hierarchy::Root(true),
      Mesh3d(meshes->add(MeshBuilder::CreateGaussianSplatsMesh()), false),
      MeshMaterial3d(materials->add(materials::GaussianSplattingMaterial::Default())),
      Transform::FromXYZ(0.0f, 0.0f, 0.0f));
  }

  void GaussianSplatsUpdateSystem::onExecute()
  {
    // Initialize XR experience reference if not already done
    if (!xrExperience_)
      xrExperience_ = getResource<WebXRExperience>();

    updateGlobalSplatsMesh();
  }

  void GaussianSplatsUpdateSystem::updateGlobalSplatsMesh()
  {
    // Find the global GaussianSplatsMesh entity (has Mesh3d with GaussianSplatsMesh handle)
    auto meshEntities = queryEntities<Mesh3d>([](const Mesh3d &mesh) -> bool
                                              { return mesh.is<GaussianSplatsMesh>(); });

    if (meshEntities.empty())
      return; // No global splats mesh found

    auto globalSplatsMeshEntityId = meshEntities[0];
    auto meshComponent = getComponent<Mesh3d>(globalSplatsMeshEntityId);
    if (!meshComponent)
      return;

    auto splatsMesh = meshComponent->getHandleAs<GaussianSplatsMesh>();
    if (!splatsMesh)
      return;

    // Update splats from all GaussianSplattingModel3d entities
    auto modelEntities = queryEntities<GaussianSplattingModel3d>([](const GaussianSplattingModel3d &model) -> bool
                                                                 { return model.isLoaded() && model.visible(); });

    for (auto entityId : modelEntities)
    {
      // Add/update this entity in the global mesh
      splatsMesh->updateSplatsEntity(entityId);
    }

    // Get view matrix for depth sorting
    glm::mat4 viewMatrix = glm::mat4(1.0f);

    // Try to get view matrix from WebXR experience first
    if (xrExperience_ != nullptr)
    {
      auto xrViewerPose = xrExperience_->viewerPose();
      if (xrViewerPose != nullptr)
      {
        auto &views = xrViewerPose->views();
        if (!views.empty())
        {
          auto firstView = views[0];
          viewMatrix = firstView->transform().matrix();
        }
      }
    }

    // Sort splats by depth using the first view (or identity matrix if no XR)
    splatsMesh->sortSplatsByDepth(viewMatrix, [this](ecs::EntityId entityId) -> GaussianSplattingModel3d *
                                  { return getComponent<GaussianSplattingModel3d>(entityId); });
  }
}