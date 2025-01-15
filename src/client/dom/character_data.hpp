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
    CharacterData(CharacterData &other);
    ~CharacterData() = default;

  public:
    std::string &data();
    const size_t length() const;
    std::shared_ptr<Element> nextElementSibling();
    std::shared_ptr<Element> previousElementSibling();

  protected:
    std::string data_;
    std::weak_ptr<Element> nextElementSibling_;
    std::weak_ptr<Element> previousElementSibling_;
  };
}
