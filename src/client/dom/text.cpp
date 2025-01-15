#include "./text.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;
  using namespace pugi;

  // Create a text node from a document and a value.
  xml_node createTextNode(shared_ptr<xml_document> doc, const string &value = "")
  {
    xml_node textNode(node_pcdata, doc.get());
    if (value != "" && !textNode.set_value(value.c_str()))
      return xml_node();
    return textNode;
  }

  Text::Text(xml_node node, shared_ptr<Document> ownerDocument)
      : CharacterData(node, ownerDocument)
  {
  }

  Text::Text(shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal), ownerDocument)
  {
  }

  Text::Text(const string value, shared_ptr<Document> ownerDocument)
      : Text(createTextNode(ownerDocument->docInternal, value), ownerDocument)
  {
  }

  Text::Text(CharacterData &other)
      : CharacterData(other)
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
}
