#include <functional>
#include <client/cssom/selectors/css_selector_parser.hpp>
#include <client/cssom/selectors/matching.hpp>

#include "./document_fragment.hpp"
#include "./document.hpp"
#include "./node_list-inl.hpp"

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

  shared_ptr<Node> DocumentFragment::CloneDocumentFragment(shared_ptr<Node> srcFragment)
  {
    auto fragmentNode = dynamic_pointer_cast<DocumentFragment>(srcFragment);
    assert(fragmentNode != nullptr && "The source node is not a document fragment.");
    return make_shared<DocumentFragment>(*fragmentNode);
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

  shared_ptr<Element> DocumentFragment::querySelector(const string &selectors)
  {
    auto s = client_cssom::selectors::CSSelectorParser::parseSelectors(selectors);
    if (s == nullopt)
      throw runtime_error("Failed to parse the CSS selectors: " + selectors);

    const auto &selectorList = s.value();

    // Helper function for recursive search
    function<shared_ptr<Element>(shared_ptr<Node>)> searchInNode = [&](shared_ptr<Node> node) -> shared_ptr<Element>
    {
      if (!Node::Is<Element>(node))
        return nullptr;

      auto element = Node::As<Element>(node);
      if (Node::Is<HTMLElement>(element))
      {
        if (client_cssom::selectors::matchesSelectorList(selectorList, Node::As<HTMLElement>(element)))
          return element;
      }

      // Search in children
      for (const auto &child : node->childNodes)
      {
        auto found = searchInNode(child);
        if (found != nullptr)
          return found;
      }
      return nullptr;
    };

    // Search in direct children and their descendants
    for (const auto &childNode : childNodes)
    {
      auto found = searchInNode(childNode);
      if (found != nullptr)
        return found;
    }
    return nullptr;
  }

  NodeList<Element> DocumentFragment::querySelectorAll(const string &selectors)
  {
    auto s = client_cssom::selectors::CSSelectorParser::parseSelectors(selectors);
    if (s == nullopt)
      throw runtime_error("Failed to parse the CSS selectors: " + selectors);

    NodeList<Element> elements(false);
    const auto &selectorList = s.value();

    // Helper function for recursive search
    function<void(shared_ptr<Node>)> searchInNode = [&](shared_ptr<Node> node)
    {
      if (!Node::Is<Element>(node))
        return;

      auto element = Node::As<Element>(node);
      if (Node::Is<HTMLElement>(element))
      {
        if (client_cssom::selectors::matchesSelectorList(selectorList, Node::As<HTMLElement>(element)))
          elements.push_back(element);
      }

      // Search in children
      for (const auto &child : node->childNodes)
      {
        searchInNode(child);
      }
    };

    // Search in direct children and their descendants
    for (const auto &childNode : childNodes)
    {
      searchInNode(childNode);
    }
    return elements;
  }

  NodeList<Element> DocumentFragment::children() const
  {
    NodeList<Element> elementChildren(false);
    for (const auto &childNode : childNodes)
    {
      if (childNode->nodeType == NodeType::ELEMENT_NODE)
        elementChildren.push_back(Node::As<Element>(childNode));
    }
    return elementChildren;
  }

  void DocumentFragment::append(const vector<shared_ptr<Node>> &nodes)
  {
    for (auto node : nodes)
    {
      if (node != nullptr)
        appendChild(node);
    }
  }

  void DocumentFragment::prepend(const vector<shared_ptr<Node>> &nodes)
  {
    // Insert nodes at the beginning in reverse order to maintain order
    for (auto it = nodes.rbegin(); it != nodes.rend(); it++)
    {
      auto node = *it;
      if (node != nullptr)
      {
        if (childNodes.empty())
          appendChild(node);
        else
          insertBefore(node, childNodes.front());
      }
    }
  }
}
