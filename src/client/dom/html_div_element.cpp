#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_div_element.hpp"

namespace dom
{
  void HTMLDivElement::createdCallback()
  {
    style.setProperty("width", "auto");
    style.setProperty("height", "auto");

    HTMLElement::createdCallback();
  }

  void HTMLDivElement::connectedCallback()
  {
    HTMLElement::connectedCallback();

    // TODO: implement connectedCallback for HTMLDivElement
    assert(entity_.has_value());
    {
      using namespace builtin_scene;

      auto sceneRef = scene();
      auto meshes = sceneRef->getResource<Meshes>();
      auto materials = sceneRef->getResource<Materials>();
      sceneRef->addComponent(entity_.value(),
                             Mesh3d(meshes->add(MeshBuilder::CreateBox(1.0f, 1.0f, 0.01f))),
                             MeshMaterial3d(materials->add(Material::Make<materials::NormalMaterial>())));
    }
  }
}
