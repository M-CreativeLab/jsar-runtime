#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <client/cssom/media_queries.hpp>
#include <client/cssom/computed_style.hpp>
#include <client/dom/node.hpp>
#include <client/dom/text.hpp>
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
          reset_style_(GetDefaultStyleRef(element_or_text_node)),
          in_media_query_(false),
          in_container_query_(false)
    {
    }

  public:
    inline float fontSize() const { return device_.rootFontSize(); }
    inline int fontWeight() const { return 400; }
    inline float lineHeight() const { return device_.rootLineHeight(); }
    inline glm::uvec4 baseViewport() const { return glm::uvec4(1920, 1080, 0, 0); }

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
    static const std::optional<ComputedStyle> GetDefaultStyleRef(std::shared_ptr<dom::Node> element_or_text_node)
    {
      if (element_or_text_node->isHTMLElement())
      {
        auto element = dynamic_pointer_cast<dom::Element>(element_or_text_node);
        if (element != nullptr)
          return element->defaultStyleRef();
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
