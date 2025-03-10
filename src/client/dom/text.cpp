#include <memory>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <client/per_process.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/text.hpp>

#include "./text.hpp"
#include "./document.hpp"
#include "./scene_object.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;
  using namespace builtin_scene;
  using namespace skia::textlayout;

  // Create a text node from a document and a value.
  xml_node createTextNode(shared_ptr<xml_document> doc, const string &value = "")
  {
    xml_node textNode(node_pcdata, doc.get());
    if (value != "" && !textNode.set_value(value.c_str()))
      return xml_node();
    return textNode;
  }

  Text::Text(xml_node node, shared_ptr<Document> ownerDocument)
      : CharacterData(node, ownerDocument),
        SceneObject(getOwnerDocumentReferenceAs<HTMLDocument>(), nodeName),
        content2d_(nullptr)
  {
    client_cssom::CSSStyleDeclaration defaultStyle;
    defaultStyle.setProperty("width", "auto");
    defaultStyle.setProperty("height", "auto");
    defaultStyle.setProperty("transform", "translateZ(5px)");
    style_ = make_shared<client_cssom::CSSStyleDeclaration>(defaultStyle.cssText());
  }

  Text::Text(shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal), ownerDocument)
  {
  }

  Text::Text(const string value, shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal, value), ownerDocument)
  {
  }

  Text::Text(Text &other)
      : CharacterData(other),
        SceneObject(other),
        content2d_(std::move(other.content2d_)),
        style_(other.style_)
  {
  }

  const string &Text::wholeText() const
  {
    return data_;
  }

  unique_ptr<Text> Text::splitText(size_t offset)
  {
    if (offset > data_.size())
      throw out_of_range("Offset is out of range");

    string first = data_.substr(0, offset);
    string second = data_.substr(offset);

    data_ = first; // Update the current text node's data
    return make_unique<Text>(second, getOwnerDocumentReference());
  }

  geometry::DOMRect Text::getTextClientRect(float maxWidth) const
  {
    if (!hasSceneComponent<WebContent>() || !hasSceneComponent<Text2d>())
      return geometry::DOMRect();

    const auto &webContentComponent = getSceneComponentChecked<WebContent>();
    const auto &textComponent = getSceneComponentChecked<Text2d>();

    string text = textComponent.content;
    auto paragraphStyle = webContentComponent.paragraphStyle();
    auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle,
                                                   TrClientContextPerProcess::Get()->getFontCacheManager());
    paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
    paragraphBuilder->addText(text.c_str(), text.size());
    paragraphBuilder->pop();

    auto paragraph = paragraphBuilder->Build();
    paragraph->layout(maxWidth > 0
                          ? maxWidth
                          : numeric_limits<float>::infinity());

    geometry::DOMRect textRect;
    textRect.width() = paragraph->getLongestLine();
    textRect.height() = paragraph->getHeight();
    return textRect;
  }

  void Text::connect()
  {
    CharacterData::connect();

    if (renderable)
    {
      SceneObject::connectedCallback(shared_from_this()); // Create the entity

      // Initialize the Content2d and connect it.
      if (content2d_ == nullptr)
        content2d_ = make_unique<Content2d>(shared_from_this());
      content2d_->onNodeConnected();

      // Append the text
      auto appendText = [this](Scene &scene)
      {
        assert(entity_.has_value());
        scene.addComponent(entity_.value(), Text2d(data()));
      };
      useScene(appendText);
    }
  }

  bool Text::adoptStyle(const client_cssom::CSSStyleDeclaration &style)
  {
    auto parentNode = getParentNodeAs<SceneObject>();
    if (parentNode != nullptr)
    {
      auto &parentStyle = parentNode->adoptedStyle_;
      std::shared_ptr<client_cssom::CSSStyleDeclaration> textStyle = nullptr;

#define _MAKE_TEXT_STYLE_IF_NOT_EXIST() \
  if (textStyle == nullptr)             \
    textStyle = make_shared<client_cssom::CSSStyleDeclaration>(style);

#define USE_PARENT_STYLE(property)                                            \
  if (parentStyle.hasProperty(property))                                      \
  {                                                                           \
    _MAKE_TEXT_STYLE_IF_NOT_EXIST()                                           \
    textStyle->setProperty(property, parentStyle.getPropertyValue(property)); \
  }

#define USE_PARENT_SIZE(property)                                                                  \
  if (parentStyle.hasProperty(property))                                                           \
  {                                                                                                \
    _MAKE_TEXT_STYLE_IF_NOT_EXIST()                                                                \
    auto dimension = parentStyle.getPropertyValueAs<crates::layout2::styles::Dimension>(property); \
    if (dimension.isLength() || dimension.isPercent())                                             \
      textStyle->setProperty(property, "100%");                                                    \
    else                                                                                           \
      textStyle->setProperty(property, "auto");                                                    \
  }

      // Font styles
      USE_PARENT_STYLE("font-family");
      USE_PARENT_STYLE("font-size");
      USE_PARENT_STYLE("font-weight");
      USE_PARENT_STYLE("font-style");
      USE_PARENT_STYLE("font-variant");
      USE_PARENT_STYLE("line-height");

      // Text styles
      USE_PARENT_STYLE("color");
      USE_PARENT_STYLE("text-align");
      USE_PARENT_STYLE("text-indent");
      USE_PARENT_STYLE("text-transform");
      USE_PARENT_STYLE("text-decoration");
      USE_PARENT_STYLE("letter-spacing");
      USE_PARENT_STYLE("word-spacing");
      USE_PARENT_STYLE("white-space");
      USE_PARENT_STYLE("direction");
      USE_PARENT_STYLE("unicode-bidi");

      // Controls the text rect
      USE_PARENT_SIZE("width");
      USE_PARENT_SIZE("height");

#undef USE_PARENT_SIZE
#undef USE_PARENT_STYLE
#undef _MAKE_TEXT_STYLE_IF_NOT_EXIST

      // Adopt the text style if it is not empty.
      if (textStyle != nullptr)
        return SceneObject::adoptStyleOn(*this, *textStyle);
    }

    // By default, just bypass the style to the scene object.
    return SceneObject::adoptStyleOn(*this, style);
  }
}
