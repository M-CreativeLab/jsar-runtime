#pragma once

#include <string>
#include "./character_data.hpp"

namespace dom
{
  class Element;
  class Document;

  class Text : public CharacterData
  {
  public:
    /**
     * Create a text node from the internal xml_node.
     *
     * @param node The internal xml_node.
     * @param ownerDocument The owner document.
     * @returns The created text node.
     */
    inline static std::shared_ptr<Text> CreateText(pugi::xml_node node,
                                                   std::shared_ptr<Document> ownerDocument)
    {
      return std::make_shared<Text>(node, ownerDocument);
    }

  public:
    Text(pugi::xml_node node, std::shared_ptr<Document> ownerDocument);
    Text(std::shared_ptr<Document> ownerDocument);
    Text(const std::string value, std::shared_ptr<Document> ownerDocument);
    Text(CharacterData &other);
    ~Text() = default;

  public:
    const std::string &wholeText() const;

  public:
    std::unique_ptr<Text> splitText(size_t offset);
  };
}
