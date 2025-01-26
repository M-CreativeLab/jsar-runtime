#pragma once

#include <string>
#include <optional>
#include "./html_element.hpp"
#include "./scene_object.hpp"

namespace dom
{
  class HTMLPlaneElement : public HTMLElement,
                           public SceneObject
  {
  public:
    HTMLPlaneElement(std::shared_ptr<Document> ownerDocument);
    HTMLPlaneElement(std::string tagName, std::shared_ptr<Document> ownerDocument);
    HTMLPlaneElement(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    HTMLPlaneElement(HTMLPlaneElement &that);

  public:
    void connectedCallback() override;
    bool renderElement(builtin_scene::Scene &scene) override;
    bool adoptStyle(const client_cssom::CSSStyleDeclaration &style) override;

  protected:
    /**
     * Update the layout style from this element. It will update the layout style from the adopted style of
     * this element.
     */
    bool updateLayoutStyle();
  };
}
