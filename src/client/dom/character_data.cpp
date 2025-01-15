#include "./character_data.hpp"
#include "./document.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;

  CharacterData::CharacterData(pugi::xml_node node, shared_ptr<Document> ownerDocument)
      : Node(node, ownerDocument)
  {
    if (node.type() == pugi::xml_node_type::node_pcdata)
      data_ = node.value();
    else
      throw invalid_argument("The node type is not a character data node.");
  }

  CharacterData::CharacterData(CharacterData &other)
      : Node(other), data_(other.data_)
  {
  }

  string &CharacterData::data()
  {
    return data_;
  }

  const size_t CharacterData::length() const
  {
    return data_.length();
  }

  shared_ptr<Element> CharacterData::nextElementSibling()
  {
    return nullptr;
  }

  shared_ptr<Element> CharacterData::previousElementSibling()
  {
    return nullptr;
  }
}
