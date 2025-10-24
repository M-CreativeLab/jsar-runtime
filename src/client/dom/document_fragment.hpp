#pragma once

#include <memory>
#include <string>

#include "./node.hpp"
#include "./element.hpp"
#include "./node_list.hpp"

namespace endor
{
  namespace dom
  {
    class Document;
    class DocumentFragment : public Node
    {
    public:
      DocumentFragment(std::shared_ptr<Document> ownerDocument);
      DocumentFragment(const DocumentFragment &other);
      ~DocumentFragment() = default;

      /**
     * Clone the given document fragment and return a new document fragment with the same properties.
     *
     * @param srcFragment The document fragment to clone.
     * @returns The cloned document fragment in `std::shared_ptr<Node>`.
     */
      static std::shared_ptr<Node> CloneDocumentFragment(std::shared_ptr<Node> srcFragment);

    public:
      bool isDocumentFragment() const override
      {
        return true;
      }
      size_t childElementCount() const;
      std::shared_ptr<Element> firstElementChild() const;
      std::shared_ptr<Element> lastElementChild() const;

      // ParentNode interface methods
      std::shared_ptr<Element> querySelector(const std::string &selectors);
      NodeList<Element> querySelectorAll(const std::string &selectors);
      NodeList<Element> children() const;

      // Convenience methods for adding multiple nodes
      void append(const std::vector<std::shared_ptr<Node>> &nodes);
      void prepend(const std::vector<std::shared_ptr<Node>> &nodes);
    };
  }
} // namespace endor
