#include "./document_fragment.hpp"
#include "./document.hpp"

namespace dom
{
  using namespace std;

  DocumentFragment::DocumentFragment(shared_ptr<Document> ownerDocument)
      : Node(NodeType::DOCUMENT_FRAGMENT_NODE, "#document-fragment", ownerDocument)
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
}
