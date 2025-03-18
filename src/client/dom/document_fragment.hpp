#pragma once

#include <memory>

#include "./node.hpp"

namespace dom
{
  class Document;
  class DocumentFragment : public Node
  {
  public:
    DocumentFragment(std::shared_ptr<Document> ownerDocument);
    DocumentFragment(const DocumentFragment &other);
    ~DocumentFragment() = default;

  public:
    size_t childElementCount() const;
  };
}
