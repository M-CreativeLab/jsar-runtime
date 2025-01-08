#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_plane_element.hpp"

namespace dom
{
  void HTMLPlaneElement::connectedCallback()
  {
    HTMLElement::connectedCallback();

    // Implement connectedCallback for HTMLPlaneElement
    assert(entity_.has_value());
    {
      using namespace builtin_scene;

      auto sceneRef = scene();
      auto meshes = sceneRef->getResource<Meshes>();
      auto materials = sceneRef->getResource<Materials>();

      {
        auto defaultMaterial = materials::ColorMaterial::White();
        sceneRef->addComponent(entity_.value(),
                               Mesh3d(meshes->add(MeshBuilder::CreateBox(1.0f, 1.0f, 0.001f))),
                               MeshMaterial3d(materials->add(defaultMaterial)));
      }
    }
  }
}
