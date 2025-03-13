#pragma once

#include <memory>

#include "./node.hpp"
#include "./document.hpp"

namespace dom
{
  class DocumentFragment : public Node
  {
  public:
    DocumentFragment(std::shared_ptr<Document> ownerDocument);
    ~DocumentFragment() = default;
  };
}
