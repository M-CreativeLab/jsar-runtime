#include <memory>
#include <skia/modules/skparagraph/include/Paragraph.h>
#include <skia/modules/skparagraph/include/ParagraphBuilder.h>
#include <client/per_process.hpp>
#include <client/builtin_scene/web_content.hpp>
#include <client/builtin_scene/text.hpp>
#include <client/cssom/values/computed/context.hpp>

#include "./text.hpp"
#include "./document.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;
  using namespace builtin_scene;
  using namespace client_cssom;
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
    defaultStyle.setProperty("transform", "translateZ(5px)"); // Avoid the z-fighting issue.
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

  void Text::connectedCallback()
  {
    CharacterData::connectedCallback();

    initCSSBoxes();
    adoptStyleDirectly(ComputedStyle::Make(defaultStyle_, shared_from_this()));
  }

  void Text::disconnectedCallback()
  {
    CharacterData::disconnectedCallback();
    resetCSSBoxes();
  }

  void Text::nodeValueChangedCallback(const std::string &newValue)
  {
    CharacterData::data() = newValue;
    Node::markAsDirty();

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
    if (ownerDocument != nullptr && isRenderable())
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
        (TR_UNLIKELY(ownerDocument == nullptr) || !isRenderable()))
      return;

    assert(ownerDocument != nullptr && "The owner document is not set when resetting CSS boxes.");
    auto &layoutView = ownerDocument->layoutViewRef();
    for (auto &box : textBoxes_)
      layoutView.removeObject(box);
    textBoxes_.clear();
  }

  bool Text::adoptStyle(const client_cssom::ComputedStyle &new_style)
  {
    if (adoptedStyle_ != nullptr && // Pass if `adoptedStyle_` is not set.
        ComputedStyle::ComputeDifference(&new_style, adoptedStyle_.get()) == ComputedStyle::kEqual)
      return false;
    return adoptStyleDirectly(new_style);
  }

  bool Text::adoptStyleDirectly(const client_cssom::ComputedStyle &new_style)
  {
    adoptedStyle_ = make_unique<client_cssom::ComputedStyle>(new_style);

    // Update the layout node style.
    bool updated = false;
    for (auto box : textBoxes_)
    {
      if (box->setStyle(adoptedStyleRef()))
        updated = true;
    }
    return updated;
  }
}
