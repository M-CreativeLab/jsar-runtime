#pragma once

#include <string>

namespace dom
{
  enum class NodeType
  {
    ELEMENT_NODE = 1,
    ATTRIBUTE_NODE,
    TEXT_NODE,
    CDATA_SECTION_NODE,
    ENTITY_REFERENCE_NODE,
    ENTITY_NODE,
    PROCESSING_INSTRUCTION_NODE,
    COMMENT_NODE,
    DOCUMENT_NODE,
    DOCUMENT_TYPE_NODE,
    DOCUMENT_FRAGMENT_NODE,
    NOTATION_NODE,
  };

  class Node
  {
  public:
    Node();
    ~Node() = default;

  public:
    std::shared_ptr<Node> appendChild(std::shared_ptr<Node> node);
    std::shared_ptr<Node> cloneNode(bool deep = false);
    bool contains(std::shared_ptr<Node> node);

  public:
    std::string baseURI;
    bool isConnected;
    NodeType nodeType;
    std::string textContent;

    std::shared_ptr<Node> firstChild;
    std::shared_ptr<Node> lastChild;
    std::shared_ptr<Node> parentNode;
  };
}
