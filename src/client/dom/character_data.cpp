#include "./character_data.hpp"
#include "./document.hpp"
#include "./element.hpp"

namespace dom
{
  using namespace std;

  string replaceAll(const string &str, const string &from, const string &to)
  {
    if (str.empty() || str.size() < from.size())
      return str;

    string result = str;
    size_t start_pos = 0;
    while ((start_pos = result.find(from, start_pos)) != string::npos)
    {
      result.replace(start_pos, from.length(), to);
      start_pos += to.length();
    }
    return result;
  }

  string processTextContent(const char *text)
  {
    if (text == nullptr || strlen(text) == 0)
      return "";

    string result(text);
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

  CharacterData::CharacterData(const CharacterData &other)
      : Node(other),
        data_(other.data_),
        nextElementSibling_(other.nextElementSibling_),
        previousElementSibling_(other.previousElementSibling_)
  {
  }

  shared_ptr<Element> CharacterData::nextElementSibling()
  {
    return nullptr;
  }

  shared_ptr<Element> CharacterData::previousElementSibling()
  {
    return nullptr;
  }

  void CharacterData::remove()
  {
    if (parentNode.expired())
      return;

    auto parent = parentNode.lock();
    if (parent == nullptr)
      return;

    parent->removeChild(shared_from_this());
  }

  void CharacterData::appendData(const string &data)
  {
    data_ += data;
  }

  void CharacterData::deleteData(size_t offset, size_t count)
  {
    if (offset >= data_.length())
      return;

    data_.erase(offset, count);
  }

  void CharacterData::insertData(size_t offset, const string &data)
  {
    if (offset >= data_.length())
      return;

    data_.insert(offset, data);
  }

  void CharacterData::replaceData(size_t offset, size_t count, const string &data)
  {
    if (offset >= data_.length())
      return;

    data_.replace(offset, count, data);
  }

  string CharacterData::substringData(size_t offset, size_t count)
  {
    if (offset >= data_.length())
      return "";
    return data_.substr(offset, count);
  }

  void CharacterData::before(vector<shared_ptr<Node>> nodes)
  {
    auto parent = parentNode.lock();
    if (parent == nullptr || nodes.size() == 0)
      return;

    for (auto &node : nodes)
    {
      if (node == nullptr)
        continue;
      parent->insertBefore(node, static_pointer_cast<Element>(shared_from_this()));
    }
  }

  void CharacterData::before(string text)
  {
    before(getOwnerDocumentChecked().createTextNode(text));
  }

  void CharacterData::after(vector<shared_ptr<Node>> nodes)
  {
    auto parent = parentNode.lock();
    if (parent == nullptr || nodes.size() == 0)
      return;

    auto next = nextSibling();
    for (auto &node : nodes)
    {
      if (node == nullptr)
        continue;

      parent->insertBefore(node, next);
    }
  }

  void CharacterData::after(string text)
  {
    after(getOwnerDocumentChecked().createTextNode(text));
  }
}
