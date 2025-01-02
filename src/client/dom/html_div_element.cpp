#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_div_element.hpp"

namespace dom
{
  void HTMLDivElement::connectedCallback()
  {
    HTMLElement::connectedCallback();

    // TODO: implement connectedCallback for HTMLDivElement
    assert(entity_.has_value());
    {
      using namespace builtin_scene;

      auto meshes = builtinScene_->getResource<Meshes>();
      auto materials = builtinScene_->getResource<Materials>();
      builtinScene_->addComponent(entity_.value(),
                                  Mesh3d(meshes->add(MeshBuilder::CreateBox(0.25f, 0.25f, 0.01f))),
                                  MeshMaterial3d(materials->add(Material::Make<materials::NormalMaterial>())));
    }
  }
}
