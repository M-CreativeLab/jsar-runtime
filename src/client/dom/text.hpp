#pragma once

#include <string>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>
#include <client/layout/layout_text.hpp>

#include "./html_element.hpp"
#include "./character_data.hpp"
#include "./geometry/dom_rect.hpp"

namespace dom
{
  class Node;
  class Element;
  class Document;

  class Text final : public CharacterData
  {
    friend class RenderHTMLDocument;
    friend class Element;

  public:
    /**
     * Create a text node from the internal xml_node.
     *
     * @param node The internal xml_node.
     * @param ownerDocument The owner document.
     * @returns The created text node.
     */
    static std::shared_ptr<Text> CreateText(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    /**
     * Clone the text node.
     *
     * @param srcText The source text node to clone.
     * @returns The cloned text node.
     */
    static std::shared_ptr<Node> CloneText(shared_ptr<Node> srcText);

  public:
    Text(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    Text(std::shared_ptr<Document> ownerDocument);
    Text(const std::string value, std::shared_ptr<Document> ownerDocument);
    Text(const Text &other);
    ~Text() = default;

  public:
    const std::string &wholeText() const;

  public:
    [[nodiscard]] std::unique_ptr<Text> splitText(size_t offset);

    inline bool hasAdoptedStyle() const { return adoptedStyle_ != nullptr; }
    inline const client_cssom::CSSStyleDeclaration &adoptedStyleRef() const { return *adoptedStyle_; }

  private:
    bool isText() const override final { return true; }

    void connectedCallback() override;
    void disconnectedCallback() override;
    void nodeValueChangedCallback(const std::string &newValue) override;

  private:
    void initCSSBoxes();
    void resetCSSBoxes(bool skipCheck = false);

    bool adoptStyle(const client_cssom::CSSStyleDeclaration &);
    bool adoptStyleDirectly(const client_cssom::CSSStyleDeclaration &);

  private:
    client_cssom::CSSStyleDeclaration defaultStyle_;
    std::unique_ptr<client_cssom::CSSStyleDeclaration> adoptedStyle_;
    std::vector<std::shared_ptr<client_layout::LayoutText>> textBoxes_;
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
    float offsetWidth_ = 0.0f;
    float offsetHeight_ = 0.0f;
  };
}
