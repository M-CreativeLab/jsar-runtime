#include <client/builtin_scene/ecs-inl.hpp>
#include "./html_plane_element.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;
  using namespace builtin_scene;

  HTMLPlaneElement::HTMLPlaneElement(shared_ptr<Document> ownerDocument)
      : HTMLElement("PLANE", ownerDocument),
        SceneObject(getOwnerDocumentReferenceAs<HTMLDocument>(), nodeName)
  {
  }

  HTMLPlaneElement::HTMLPlaneElement(string tagName, shared_ptr<Document> ownerDocument)
      : HTMLElement(tagName, ownerDocument),
        SceneObject(getOwnerDocumentReferenceAs<HTMLDocument>(), nodeName)
  {
  }

  HTMLPlaneElement::HTMLPlaneElement(xml_node node, shared_ptr<Document> ownerDocument)
      : HTMLElement(node, ownerDocument),
        SceneObject(getOwnerDocumentReferenceAs<HTMLDocument>(), nodeName)
  {
  }

  HTMLPlaneElement::HTMLPlaneElement(HTMLPlaneElement &that)
      : HTMLElement(that), SceneObject(that)
  {
  }

  void HTMLPlaneElement::connectedCallback()
  {
    HTMLElement::connectedCallback();
    SceneObject::connectedCallback(*this);

    // Implement connectedCallback for HTMLPlaneElement
    assert(entity_.has_value());
    auto initMeshAndMaterial = [this](Scene &scene)
    {
      auto meshes = scene.getResource<Meshes>();
      auto materials = scene.getResource<Materials>();

      auto defaultMaterial = materials::ColorMaterial::White();
      scene.addComponent(entity_.value(),
                         Mesh3d(meshes->add(MeshBuilder::CreateBox(1.0f, 1.0f, 0.001f))),
                         MeshMaterial3d(materials->add(defaultMaterial)));
    };
    useScene(initMeshAndMaterial);
  }

  void HTMLPlaneElement::renderElement(Scene &scene)
  {
    HTMLElement::renderElement(scene);
    SceneObject::render();
  }

  bool HTMLPlaneElement::adoptStyle(client_cssom::CSSStyleDeclaration &style)
  {
    if (!HTMLElement::adoptStyle(style))
      return false;
    return adoptStyleOn(*this, style);
  }
}
