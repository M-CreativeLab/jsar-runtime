#pragma once

#include <string>
#include "./character_data.hpp"

namespace dom
{
  class Comment final : public CharacterData
  {
    using CharacterData::CharacterData;

  public:
    static std::shared_ptr<Comment> CreateComment(pugi::xml_node node, std::shared_ptr<Document> ownerDocument)
    {
      return std::make_shared<Comment>(node, ownerDocument);
    }
    static std::shared_ptr<Node> CloneComment(shared_ptr<Node> srcComment)
    {
      auto commentNode = dynamic_pointer_cast<Comment>(srcComment);
      assert(commentNode != nullptr && "The source node is not a comment node.");
      return make_shared<Comment>(*commentNode);
    }

  public:
    Comment(const std::string &content = "", std::shared_ptr<Document> ownerDocument = nullptr);
  };
}
