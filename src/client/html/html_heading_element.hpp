#pragma once

#include <string>
#include "./html_element.hpp"

namespace dom
{
#define HEADING_LEVEL_MAP(XX) \
  XX(H1, "h1")                \
  XX(H2, "h2")                \
  XX(H3, "h3")                \
  XX(H4, "h4")                \
  XX(H5, "h5")                \
  XX(H6, "h6")

  enum class HeadingLevel
  {
#define XX(LEVEL, _) LEVEL,
    HEADING_LEVEL_MAP(XX)
#undef XX
  };

  inline std::string to_string(HeadingLevel level)
  {
#define XX(LEVEL, _)        \
  case HeadingLevel::LEVEL: \
    return #LEVEL;

    switch (level)
    {
      HEADING_LEVEL_MAP(XX)
    }
#undef XX
  }

  inline HeadingLevel from_string(const std::string &input)
  {
#define XX(LEVEL, TAG_NAME) \
  if (input == #TAG_NAME)   \
    return HeadingLevel::LEVEL;

    HEADING_LEVEL_MAP(XX)
#undef XX

    assert(false && "Invalid heading level string");
  }

  class HTMLHeadingElement : public HTMLElement
  {
  public:
    using HTMLElement::HTMLElement;

  public:
    HTMLHeadingElement(HeadingLevel level, std::shared_ptr<Document> ownerDocument)
        : HTMLElement(to_string(level), ownerDocument),
          level_(level)
    {
    }
    HTMLHeadingElement(std::string tagName, std::shared_ptr<Document> ownerDocument)
        : HTMLHeadingElement(from_string(tagName), ownerDocument)
    {
    }

  public:
    void createdCallback(bool from_scripting) override
    {
      defaultStyle_.setProperty("display", "block");
      defaultStyle_.setProperty("height", "auto");
      defaultStyle_.setProperty("width", "auto");
      defaultStyle_.setProperty("margin", "10px 0");
      defaultStyle_.setProperty("padding", "10px");
      defaultStyle_.setProperty("font-weight", "bold");

      switch (level_)
      {
      case HeadingLevel::H2:
        defaultStyle_.setProperty("font-size", "1.5em");
        break;
      case HeadingLevel::H3:
        defaultStyle_.setProperty("font-size", "1.17em");
        break;
      case HeadingLevel::H4:
        defaultStyle_.setProperty("font-size", "1em");
        break;
      case HeadingLevel::H5:
        defaultStyle_.setProperty("font-size", "0.83em");
        break;
      case HeadingLevel::H6:
        defaultStyle_.setProperty("font-size", "0.67em");
        break;
      case HeadingLevel::H1:
        defaultStyle_.setProperty("font-size", "2em");
      default:
        break;
      }

      HTMLElement::createdCallback(from_scripting);
    }

  private:
    HeadingLevel level_;
  };

#undef HEADING_LEVEL_MAP
}
