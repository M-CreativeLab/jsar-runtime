#include <optional>
#include <client/builtin_scene/scene.hpp>
#include <client/builtin_scene/model_3d.hpp>
#include <client/builtin_scene/materials/gaussian_splatting.hpp>

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
      Model3d &modelComponent = scene.getComponentChecked<Model3d>(entity());

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

      modelComponent.setSplats(std::move(convertedSplats));

      // Update spatial information from the HTML element
      assert(dom::Node::Is<dom::HTMLModelElement>(node()));
      auto &modelElement = dom::Node::AsChecked<dom::HTMLModelElement>(node());

      // Create and add GaussianSplattingMaterial component if this is a 3DGS model
      if (modelComponent.isGaussianSplatting())
      {
        auto material = std::make_shared<materials::GaussianSplattingMaterial>();

        // Convert builtin_scene::GaussianSplat to materials::GaussianSplattingMaterial::GaussianSplat
        std::vector<materials::GaussianSplattingMaterial::GaussianSplat> materialSplats;
        materialSplats.reserve(convertedSplats.size());

        for (const auto &splat : convertedSplats)
        {
          materials::GaussianSplattingMaterial::GaussianSplat materialSplat;
          materialSplat.position = glm::vec3(splat.position[0], splat.position[1], splat.position[2]);
          materialSplat.color = glm::vec3(splat.color[0], splat.color[1], splat.color[2]);
          materialSplat.opacity = splat.opacity;
          materialSplat.scale = glm::vec3(splat.scale[0], splat.scale[1], splat.scale[2]);
          materialSplat.rotation = glm::vec4(splat.rotation[0], splat.rotation[1], splat.rotation[2], splat.rotation[3]);
          materialSplats.push_back(materialSplat);
        }

        material->updateSplats(materialSplats);

        // Add the material as a component (assuming there's a component for this)
        // Note: This may need adjustment based on how materials are handled in the ECS
        scene.addComponent(entity(), material);
      }
    };
    useSceneWithCallback(setModelData);
  }

  void LayoutModel3d::entityDidCreate(ecs::EntityId entity)
  {
    LayoutReplaced::entityDidCreate(entity);

    auto addModelComponent = [this, &entity](Scene &scene)
    {
      assert(dom::Node::Is<dom::HTMLModelElement>(node()));
      auto &modelElement = dom::Node::AsChecked<dom::HTMLModelElement>(node());

      // Determine model type from the element
      Model3d::ModelType modelType = Model3d::ModelType::Unknown;
      if (modelElement.getType() == "3dgs")
      {
        modelType = Model3d::ModelType::GaussianSplatting;
      }

      scene.addComponent(entity, Model3d(modelElement.getSrc(), modelType));
      
      // Add GaussianSplattingMaterial component if this is a 3DGS model
      if (modelType == Model3d::ModelType::GaussianSplatting)
      {
        auto material = std::make_shared<materials::GaussianSplattingMaterial>();
        scene.addComponent(entity, material);
      }
    };
    useSceneWithCallback(addModelComponent);
  }

  void LayoutModel3d::entityWillBeDestroyed(ecs::EntityId entity)
  {
    auto removeModelComponent = [&entity](Scene &scene)
    {
      scene.removeComponent<Model3d>(entity);
      // Also remove any material components if they exist
      scene.removeComponent<std::shared_ptr<materials::GaussianSplattingMaterial>>(entity);
    };
    useSceneWithCallback(removeModelComponent);

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
      Model3d &modelComponent = scene.getComponentChecked<Model3d>(entity());
      modelComponent.setVisible(b);
    };
    useSceneWithCallback(setVisible);

    last_visible_ = b;
  }
  }
}