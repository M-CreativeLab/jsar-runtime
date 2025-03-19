#include "./document_fragment.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;

  DocumentFragment::DocumentFragment(shared_ptr<Document> ownerDocument)
      : Node(NodeType::DOCUMENT_FRAGMENT_NODE, "#document-fragment", ownerDocument)
  {
  }

  DocumentFragment::DocumentFragment(const DocumentFragment &other)
      : Node(other)
  {
  }

  size_t DocumentFragment::childElementCount() const
  {
    size_t count = 0;
    for (auto childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        count++;
    }
    return count;
  }

  shared_ptr<Element> DocumentFragment::firstElementChild() const
  {
    for (auto childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        return Node::As<Element>(childNode);
    }
    return nullptr;
  }

  shared_ptr<Element> DocumentFragment::lastElementChild() const
  {
    for (auto it = childNodes.rbegin(); it != childNodes.rend(); it++)
    {
      shared_ptr<Node> childNode = *it;
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        return Node::As<Element>(childNode);
    }
    return nullptr;
  }
}
