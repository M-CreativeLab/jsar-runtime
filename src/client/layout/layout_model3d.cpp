#include <optional>
#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/gaussian_splatting.hpp>
#include <client/builtin_scene/gaussian_splats_mesh.hpp>
#include <client/builtin_scene/materials/gaussian_splatting.hpp>
#include <client/builtin_scene/meshes/builder.hpp>
#include <client/builtin_scene/renderer/scene_renderer.hpp>

#include "./layout_model3d.hpp"
#include "./layout_view.hpp"

namespace client_layout
{
  using namespace std;
  using namespace dom;
  using namespace builtin_scene;
  using namespace crates::layout2::styles;

  void LayoutModel3d::setModelData(const std::vector<dom::HTMLModelElement::GaussianSplat> &splats)
  {
    auto setModelData = [this, &splats](Scene &scene)
    {
      GaussianSplattingModel &splatComponent = scene.getComponentChecked<GaussianSplattingModel>(entity());

      // Convert HTMLModelElement::GaussianSplat to builtin_scene::GaussianSplat
      std::vector<builtin_scene::GaussianSplat> convertedSplats;
      convertedSplats.reserve(splats.size());

      for (const auto &splat : splats)
      {
        builtin_scene::GaussianSplat convertedSplat;
        convertedSplat.position[0] = splat.position[0];
        convertedSplat.position[1] = splat.position[1];
        convertedSplat.position[2] = splat.position[2];
        convertedSplat.color[0] = splat.color[0];
        convertedSplat.color[1] = splat.color[1];
        convertedSplat.color[2] = splat.color[2];
        convertedSplat.opacity = splat.opacity;
        convertedSplat.scale[0] = splat.scale[0];
        convertedSplat.scale[1] = splat.scale[1];
        convertedSplat.scale[2] = splat.scale[2];
        convertedSplat.rotation[0] = splat.rotation[0];
        convertedSplat.rotation[1] = splat.rotation[1];
        convertedSplat.rotation[2] = splat.rotation[2];
        convertedSplat.rotation[3] = splat.rotation[3];
        convertedSplats.push_back(convertedSplat);
      }

      splatComponent.setSplats(std::move(convertedSplats));

      // Add splats to the global GaussianSplatsMesh
      // Find the global GaussianSplatsMesh entity
      auto globalSplatsMeshEntities = scene.template queryEntities<GaussianSplatsMesh>([](const GaussianSplatsMesh &mesh) -> bool
                                                                                       { return true; });
      if (!globalSplatsMeshEntities.empty())
      {
        auto globalSplatsMeshEntityId = globalSplatsMeshEntities[0];
        auto &globalSplatsMesh = scene.getComponentChecked<GaussianSplatsMesh>(globalSplatsMeshEntityId);
        globalSplatsMesh.addSplatsFromEntity(entity(), splatComponent.getSplats());
      }
      else
      {
        // Create global GaussianSplatsMesh if it doesn't exist
        auto globalEntity = scene.spawn();
        scene.addComponent(globalEntity, GaussianSplatsMesh());
        auto &globalSplatsMesh = scene.getComponentChecked<GaussianSplatsMesh>(globalEntity);

        // Initialize the mesh with GL context from renderer resource
        auto renderer = scene.template getResource<SceneRenderer>();
        if (renderer)
        {
          globalSplatsMesh.initializeGeometry(renderer->glContext());
        }
        globalSplatsMesh.addSplatsFromEntity(entity(), splatComponent.getSplats());
      }
    };
    useSceneWithCallback(setModelData);
  }

  void LayoutModel3d::entityDidCreate(ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreate(entity);

    auto addSplatComponent = [this, &entity](Scene &scene)
    {
      assert(dom::Node::Is<dom::HTMLModelElement>(node()));
      auto &modelElement = dom::Node::AsChecked<dom::HTMLModelElement>(node());

      // Create GaussianSplattingModel for this entity
      scene.addComponent(entity, GaussianSplattingModel(modelElement.getSrc()));
    };
    useSceneWithCallback(addSplatComponent);
  }

  void LayoutModel3d::entityWillBeDestroyed(ecs::EntityId entity)
  {
    auto removeSplatComponent = [this, &entity](Scene &scene)
    {
      // Remove splats from global mesh before destroying component
      auto globalSplatsMeshEntities = scene.template queryEntities<GaussianSplatsMesh>([](const GaussianSplatsMesh &mesh) -> bool
                                                                                       { return true; });
      if (!globalSplatsMeshEntities.empty())
      {
        auto globalSplatsMeshEntityId = globalSplatsMeshEntities[0];
        auto &globalSplatsMesh = scene.getComponentChecked<GaussianSplatsMesh>(globalSplatsMeshEntityId);
        globalSplatsMesh.removeSplatsFromEntity(entity);
      }

      scene.removeComponent<GaussianSplattingModel>(entity);
    };
    useSceneWithCallback(removeSplatComponent);

    LayoutReplaced::entityWillBeDestroyed(entity);
  }

  void LayoutModel3d::styleWillChange(client_cssom::ComputedStyle &new_style)
  {
    LayoutReplaced::styleWillChange(new_style);
  }

  void LayoutModel3d::didComputeLayoutOnce(const ConstraintSpace &avilable_space)
  {
    LayoutReplaced::didComputeLayoutOnce(avilable_space);

    Fragment fragment = this->fragment();
    if (!is_layout_ready_)
    {
      layoutDidFirstReady(fragment);
      is_layout_ready_ = true;
    }

    bool shouldVisible = fragment.visibleInViewport(viewRef().viewport);
    if (shouldVisible)
      Node::AsChecked<dom::HTMLModelElement>(node()).loadModel();

    setVisible(shouldVisible);
  }

  void LayoutModel3d::sizeDidChange(const Fragment &newSize)
  {
    LayoutReplaced::sizeDidChange(newSize);
  }

  void LayoutModel3d::layoutDidFirstReady(const Fragment &fragment)
  {
  }

  void LayoutModel3d::setVisible(bool b)
  {
    auto setVisible = [this, &b](Scene &scene)
    {
      GaussianSplattingModel &splatComponent = scene.getComponentChecked<GaussianSplattingModel>(entity());
      splatComponent.setVisible(b);
    };
    useSceneWithCallback(setVisible);

    last_visible_ = b;
  }

  bool LayoutModel3d::enableCustomGeometry() const
  {
    return true;
  }
}
}