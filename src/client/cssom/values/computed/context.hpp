#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <client/browser/window.hpp>
#include <client/cssom/media_queries.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/dom/node.hpp>
#include <client/dom/text.hpp>
#include <client/dom/document.hpp>
#include <client/html/html_element.hpp>

namespace client_cssom::values::computed
{
  // Context for computed value conversion.
  class Context
  {
  public:
    static Context From(std::shared_ptr<dom::Node> element_or_text_node)
    {
      return Context(element_or_text_node);
    }

  public:
    Context(std::shared_ptr<dom::Node> element_or_text_node)
        : element_or_text_node_(element_or_text_node),
          device_(GetDevice(element_or_text_node)),
          reset_style_(GetDefaultStyleRef(element_or_text_node)),
          in_media_query_(false),
          in_container_query_(false)
    {
    }

  public:
    inline float rootFontSize() const { return device_.rootFontSize(); }
    float baseFontSize() const
    {
      std::shared_ptr<dom::Node> node = element_or_text_node_.lock();
      if (TR_UNLIKELY(node == nullptr) || !node->isElementOrText())
        return rootFontSize();

      if (node->isElement())
      {
        auto element = dynamic_pointer_cast<dom::Element>(node);
        assert(element != nullptr && "The node should be an Element.");
        return element->adoptedStyleRef().fontSize().computedSize().px();
      }
      else if (node->isText())
      {
        // For text nodes, we use the font size of the parent element.
        auto text = dynamic_pointer_cast<dom::Text>(node);
        assert(text != nullptr && "The parent element should not be null.");
        return text->adoptedStyleRef().fontSize().computedSize().px();
      }

      // Unreachable case
      assert(false && "Unreachable");
    }
    inline int baseFontWeight() const { return 400; }
    inline float baseLineHeight() const
    {
      std::shared_ptr<dom::Node> node = element_or_text_node_.lock();
      if (TR_UNLIKELY(node == nullptr) || !node->isElementOrText())
        return rootLineHeight();

      if (node->isElement())
      {
        auto element = dynamic_pointer_cast<dom::Element>(node);
        assert(element != nullptr && "The node should be an Element.");
        return element->adoptedStyleRef().lineHeight().computedSize(baseFontSize());
      }
      else if (node->isText())
      {
        // For text nodes, we use the font size of the parent element.
        auto text = dynamic_pointer_cast<dom::Text>(node);
        assert(text != nullptr && "The parent element should not be null.");
        return text->adoptedStyleRef().lineHeight().computedSize(baseFontSize());
      }

      // Unreachable case
      assert(false && "Unreachable");
    }
    inline float rootLineHeight() const { return device_.rootLineHeight(); }
    inline glm::uvec4 baseViewport() const
    {
      auto device_viewport = device_.viewportSize();
      return glm::uvec4(device_viewport.x, device_viewport.y, 0, 0);
    }

    // Returns this reference as `HTMLElement` object, otherwise throws an exception.
    const dom::HTMLElement &elementRef() const
    {
      std::shared_ptr<dom::Node> node = element_or_text_node_.lock();
      if (TR_UNLIKELY(node == nullptr))
        throw std::runtime_error("The internal `element_or_text_node` is null.");

      if (TR_LIKELY(node->isHTMLElement()))
      {
        auto html_element = dynamic_pointer_cast<dom::HTMLElement>(node);
        return *html_element;
      }
      else
      {
        throw std::runtime_error("The node is not an HTMLElement.");
      }
    }
    // Returns this reference as `Text` object, otherwise throws an exception.
    const dom::Text &textRef() const
    {
      std::shared_ptr<dom::Node> node = element_or_text_node_.lock();
      if (TR_UNLIKELY(node == nullptr))
        throw std::runtime_error("The internal `element_or_text_node` is null.");

      if (TR_LIKELY(node->isText()))
      {
        auto text_node = dynamic_pointer_cast<dom::Text>(node);
        return *text_node;
      }
      else
      {
        throw std::runtime_error("The node is not a Text node.");
      }
    }

    // Returns the parent element of the current element or text node.
    inline std::shared_ptr<dom::Element> parentElement() const
    {
      if (auto node = element_or_text_node_.lock())
        return node->getParentElement();
      return nullptr;
    }

    inline const std::optional<ComputedStyle> resetStyle() const { return reset_style_; }
    // The parent element's adopted style should be inherited by the child element.
    // This method returns the inherited style from the parent element.
    inline const std::optional<ComputedStyle> inheritedStyle() const
    {
      auto parent_element = parentElement();
      if (parent_element == nullptr)
        return ComputedStyle();
      else
        return parent_element->adoptedStyleRef();
    }

  private:
    static const Device &GetDevice(std::shared_ptr<dom::Node> element_or_text_node)
    {
      std::shared_ptr<browser::Window> window;
      if (element_or_text_node->isDocument())
      {
        window = dynamic_pointer_cast<dom::Document>(element_or_text_node)->defaultView();
      }
      else
      {
        const dom::Document &owner_document = element_or_text_node->getOwnerDocumentChecked();
        window = owner_document.defaultView();
      }
      return window->device();
    }
    static const std::optional<ComputedStyle> GetDefaultStyleRef(std::shared_ptr<dom::Node> element_or_text_node)
    {
      if (element_or_text_node->isHTMLElement())
      {
        auto element = dynamic_pointer_cast<dom::Element>(element_or_text_node);
        if (element != nullptr)
          return ComputedStyle(element->defaultStyleRef(), std::nullopt);
      }
      return std::nullopt; // No default style for text nodes.
    }

  private:
    Device device_;
    std::weak_ptr<dom::Node> element_or_text_node_;
    std::optional<ComputedStyle> reset_style_;

    bool in_media_query_;
    bool in_container_query_;
  };
}
