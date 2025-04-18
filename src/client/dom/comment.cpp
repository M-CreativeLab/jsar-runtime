#include "./comment.hpp"

namespace dom
{
  using namespace std;

  Comment::Comment(const std::string &content, shared_ptr<Document> ownerDocument)
      : CharacterData(NodeType::COMMENT_NODE, "#comment",
                      "<!--" + content + "-->",
                      ownerDocument)
  {
  }
}
