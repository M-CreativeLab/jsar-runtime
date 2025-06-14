#pragma once

#include <memory>

#include "./node.hpp"
#include "./element.hpp"

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
    bool isDocumentFragment() const override
    {
      return true;
    }
    size_t childElementCount() const;
    std::shared_ptr<Element> firstElementChild() const;
    std::shared_ptr<Element> lastElementChild() const;
  };
}
