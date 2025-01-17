#include "./character_data.hpp"
#include "./document.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;

  string replaceAll(const string &str, const string &from, const string &to)
  {
    string result = str;
    size_t start_pos = 0;
    while ((start_pos = result.find(from, start_pos)) != string::npos)
    {
      result.replace(start_pos, from.length(), to);
      start_pos += to.length();
    }
    return result;
  }

  string processTextContent(const string &text)
  {
    string result = text;
    // Remove the new line characters
    result = replaceAll(result, "\n", "");
    result = replaceAll(result, "\r", "");
    // Replace the special characters
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&amp;", "&");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&apos;", "'");
    // Trim the beginning and ending spaces
    result = result.substr(result.find_first_not_of(" \t\n\r"), result.find_last_not_of(" \t\n\r") + 1);
    return result;
  }

  CharacterData::CharacterData(pugi::xml_node node, shared_ptr<Document> ownerDocument)
      : Node(node, ownerDocument)
  {
    if (node.type() == pugi::xml_node_type::node_pcdata)
      data_ = processTextContent(node.value());
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
