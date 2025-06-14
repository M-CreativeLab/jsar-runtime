#pragma once

#include <string>
#include "./node.hpp"

namespace dom
{
  class Element;
  class Document;

  class CharacterData : public Node
  {
  public:
    CharacterData(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    CharacterData(NodeType nodeType, std::string nodeName, const std::string &data, std::shared_ptr<Document> ownerDocument);
    CharacterData(const CharacterData &other);
    ~CharacterData() = default;

  public:
    std::string &data()
    {
      return data_;
    }
    const size_t length() const
    {
      return data_.length();
    }
    std::shared_ptr<Element> nextElementSibling();
    std::shared_ptr<Element> previousElementSibling();

  public:
    void remove();
    void appendData(const std::string &data);
    void deleteData(size_t offset, size_t count);
    void insertData(size_t offset, const std::string &data);
    void replaceData(size_t offset, size_t count, const std::string &data);
    std::string substringData(size_t offset, size_t count);
    void before(std::vector<std::shared_ptr<Node>> nodes);
    void before(std::string text);
    inline void before(std::shared_ptr<Node> node)
    {
      before(std::vector<std::shared_ptr<Node>>{node});
    }
    void after(std::vector<std::shared_ptr<Node>> nodes);
    void after(std::string text);
    inline void after(std::shared_ptr<Node> node)
    {
      after(std::vector<std::shared_ptr<Node>>{node});
    }

  private:
    bool isCharacterData() const override final
    {
      return true;
    }

  protected:
    std::string data_;
    std::weak_ptr<Element> nextElementSibling_;
    std::weak_ptr<Element> previousElementSibling_;
  };
}
