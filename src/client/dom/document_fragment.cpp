#include "./document_fragment.hpp"

namespace dom
{
  using namespace std;

  DocumentFragment::DocumentFragment(shared_ptr<Document> ownerDocument)
      : Node(NodeType::DOCUMENT_FRAGMENT_NODE, "#document-fragment", ownerDocument)
  {
  }
}
