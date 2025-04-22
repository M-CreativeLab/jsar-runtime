#include <memory>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <client/per_process.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/text.hpp>

#include "./text.hpp"
#include "./document.hpp"
#include "./element.hpp"

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
    if (!textNode.set_value(value.c_str()))
      return xml_node();
    return textNode;
  }

  // Remove the leading and trailing whitespaces, and \n, \r, \t characters.
  string processTextContent(const string &text)
  {
    string result(text);
    size_t start = result.find_first_not_of(" \t\n\r");
    if (start == string::npos || start >= result.length())
      start = 0;
    size_t end = result.find_last_not_of(" \t\n\r");
    if (end == string::npos || end >= result.length())
      end = result.length();
    return result.substr(start, end - start + 1);
  }

  std::shared_ptr<Text> Text::CreateText(pugi::xml_node node, shared_ptr<Document> ownerDocument)
  {
    return make_shared<Text>(node, ownerDocument);
  }

  std::shared_ptr<Node> Text::CloneText(shared_ptr<Node> srcText)
  {
    auto textNode = dynamic_pointer_cast<Text>(srcText);
    if (textNode != nullptr)
      return make_shared<Text>(*textNode);

    auto characterDataNode = dynamic_pointer_cast<CharacterData>(srcText);
    if (characterDataNode != nullptr)
      return make_shared<CharacterData>(*characterDataNode);

    return nullptr;
  }

  Text::Text(xml_node node, shared_ptr<Document> ownerDocument)
      : CharacterData(node, ownerDocument)
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

  Text::Text(const Text &other)
      : CharacterData(other),
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

  const geometry::DOMRect Text::getTextClientRect(float maxWidth) const
  {
    // if (!hasSceneComponent<WebContent>() || !hasSceneComponent<Text2d>())
    //   return geometry::DOMRect();

    // const auto &webContentComponent = getSceneComponentChecked<WebContent>();
    // const auto &textComponent = getSceneComponentChecked<Text2d>();

    // string text = textComponent.content;
    // if (text.size() == 0)
    //   return geometry::DOMRect();

    // auto paragraphStyle = webContentComponent.paragraphStyle();
    // auto paragraphBuilder = ParagraphBuilder::make(paragraphStyle,
    //                                                TrClientContextPerProcess::Get()->getFontCacheManager());
    // paragraphBuilder->pushStyle(paragraphStyle.getTextStyle());
    // paragraphBuilder->addText(text.c_str(), text.size());
    // paragraphBuilder->pop();

    // auto paragraph = paragraphBuilder->Build();
    // paragraph->layout(maxWidth > 0
    //                       ? maxWidth
    //                       : numeric_limits<float>::infinity());

    // geometry::DOMRect textRect;
    // textRect.width() = paragraph->getLongestLine();
    // textRect.height() = paragraph->getHeight();
    // return textRect;

    return geometry::DOMRect();
  }

  void Text::connectedCallback()
  {
    CharacterData::connectedCallback();
    initCSSBoxes();
  }

  void Text::disconnectedCallback()
  {
    CharacterData::disconnectedCallback();
    resetCSSBoxes();
  }

  void Text::nodeValueChangedCallback(const std::string &newValue)
  {
    CharacterData::data() = newValue;

    if (textBoxes_.size() >= 1)
    {
      auto textBox = dynamic_pointer_cast<client_layout::LayoutText>(textBoxes_.front());
      assert(textBox != nullptr &&
             "The text box must not be null in a TextNode().");
      textBox->textDidChange();
    }
  }

  void Text::initCSSBoxes()
  {
    auto ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    if (ownerDocument != nullptr && renderable)
    {
      auto &layoutView = ownerDocument->layoutViewRef();
      shared_ptr<client_layout::LayoutBoxModelObject> parentBox = nullptr;
      {
        auto parentElement = getParentElement();
        assert(parentElement != nullptr &&
               "The parent element must not be null in a TextNode().");
        parentBox = dynamic_pointer_cast<client_layout::LayoutBoxModelObject>(parentElement->principalBox());
        assert(parentBox != nullptr &&
               "The parent box must not be null in a TextNode().");
      }
      textBoxes_ = {layoutView.createText(getPtr<Text>(), parentBox)};
    }
  }

  void Text::resetCSSBoxes(bool skipCheck)
  {
    if (textBoxes_.empty())
      return;

    shared_ptr<HTMLDocument> ownerDocument = getOwnerDocumentReferenceAs<HTMLDocument>(false);
    if (!skipCheck &&
        (TR_UNLIKELY(ownerDocument == nullptr) || renderable == false))
      return;

    assert(ownerDocument != nullptr && "The owner document is not set when resetting CSS boxes.");
    auto &layoutView = ownerDocument->layoutViewRef();
    for (auto &box : textBoxes_)
      layoutView.removeObject(box);
    textBoxes_.clear();
  }

  bool Text::adoptStyle(const client_cssom::CSSStyleDeclaration &style)
  {
    client_cssom::CSSStyleDeclaration newStyle = style;

    // Inherit the style from the parent node if it is not set.
    auto parentNode = getParentNodeAs<Element>();
    if (parentNode != nullptr)
    {
      const auto &parentStyle = parentNode->adoptedStyle();

#define USE_PARENT_STYLE(property)                                          \
  if (parentStyle.hasProperty(property))                                    \
  {                                                                         \
    newStyle.setProperty(property, parentStyle.getPropertyValue(property)); \
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
      USE_PARENT_STYLE("-webkit-text-security");
      USE_PARENT_STYLE("text-decoration");
      USE_PARENT_STYLE("letter-spacing");
      USE_PARENT_STYLE("word-spacing");
      USE_PARENT_STYLE("white-space");
      USE_PARENT_STYLE("direction");
      USE_PARENT_STYLE("unicode-bidi");

#undef USE_PARENT_STYLE
#undef _MAKE_TEXT_STYLE_IF_NOT_EXIST
    }

    // Update the  style if these properties are not present.
    newStyle.update(defaultStyle_, true);
    if (adoptedStyle_.equals(newStyle)) // Skip if the style is the same.
      return false;

    adoptedStyle_ = newStyle;

    // Update the layout node style.
    bool updated = false;
    for (auto box : textBoxes_)
    {
      if (box->setStyle(adoptedStyle_))
        updated = true;
    }
    return updated;
  }
}
