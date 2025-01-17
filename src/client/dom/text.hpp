#pragma once

#include <string>
#include <client/builtin_scene/scene.hpp>
#include <client/cssom/css_style_declaration.hpp>

#include "./html_element.hpp"
#include "./character_data.hpp"
#include "./scene_object.hpp"
#include "./content2d.hpp"

namespace dom
{
  class Element;
  class Document;

  class Text final : public CharacterData,
                     public SceneObject
  {
    friend class RenderHTMLDocument;

  public:
    /**
     * Create a text node from the internal xml_node.
     *
     * @param node The internal xml_node.
     * @param ownerDocument The owner document.
     * @returns The created text node.
     */
    inline static std::shared_ptr<Text> CreateText(pugi::xml_node node,
                                                   std::shared_ptr<Document> ownerDocument)
    {
      return std::make_shared<Text>(node, ownerDocument);
    }

  public:
    Text(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    Text(std::shared_ptr<Document> ownerDocument);
    Text(const std::string value, std::shared_ptr<Document> ownerDocument);
    Text(Text &other);
    ~Text() = default;

  public:
    const std::string &wholeText() const;

  public:
    std::unique_ptr<Text> splitText(size_t offset);

  private:
    inline float offsetWidth() const override { return offsetWidth_; }
    inline float &offsetWidth() override { return offsetWidth_; }
    inline float offsetHeight() const override { return offsetHeight_; }
    inline float &offsetHeight() override { return offsetHeight_; }
    inline void onLayoutChanged() override { content2d().onLayoutSizeChanged(); }
    inline void onAdoptedStyleChanged() override { content2d().onAdoptedStyleChanged(); }
    void connect() override;

  private:
    // Adopt the specified style to the element.
    bool adoptStyle(const client_cssom::CSSStyleDeclaration &style);
    // Render the text node.
    inline bool renderText(builtin_scene::Scene &scene)
    {
      return SceneObject::render(*this);
    }
    // Get the content2d and expect it to be valid.
    inline Content2d &content2d()
    {
      assert(content2d_ != nullptr);
      return *content2d_;
    }

  private:
    std::unique_ptr<Content2d> content2d_;
    std::shared_ptr<client_cssom::CSSStyleDeclaration> style_;
    float offsetWidth_ = 0.0f;
    float offsetHeight_ = 0.0f;
  };
}
