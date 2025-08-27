#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/document_fragment.hpp>
#include <client/dom/document.hpp>
#include <client/dom/element.hpp>
#include <client/dom/text.hpp>

using namespace dom;
using namespace std;

TEST_CASE("DocumentFragment cloneNode creates proper DocumentFragment", "[DocumentFragment][cloneNode]")
{
  // Create a document and a document fragment
  auto document = make_shared<Document>();
  auto fragment = make_shared<DocumentFragment>(document);
  
  // Verify the original fragment type
  REQUIRE(fragment->nodeType == NodeType::DOCUMENT_FRAGMENT_NODE);
  REQUIRE(fragment->isDocumentFragment() == true);
  
  // Clone the fragment (shallow clone)
  auto cloned = fragment->cloneNode(false);
  
  // Verify the cloned node is also a DocumentFragment
  REQUIRE(cloned != nullptr);
  REQUIRE(cloned->nodeType == NodeType::DOCUMENT_FRAGMENT_NODE);
  REQUIRE(cloned->isDocumentFragment() == true);
  
  // Verify it's a different instance
  REQUIRE(cloned != fragment);
  
  // Verify it can be cast to DocumentFragment
  auto clonedFragment = dynamic_pointer_cast<DocumentFragment>(cloned);
  REQUIRE(clonedFragment != nullptr);
}

TEST_CASE("DocumentFragment cloneNode deep clone preserves children", "[DocumentFragment][cloneNode]")
{
  // Create a document and a document fragment
  auto document = make_shared<Document>();
  auto fragment = make_shared<DocumentFragment>(document);
  
  // Add some child nodes to the fragment
  auto textNode = make_shared<Text>("Hello", document);
  auto element = Element::CreateElement("div", document, false);
  auto childText = make_shared<Text>("World", document);
  
  fragment->appendChild(textNode);
  fragment->appendChild(element);
  element->appendChild(childText);
  
  // Verify original fragment has children
  REQUIRE(fragment->childNodes.size() == 2);
  
  // Clone the fragment with deep=true
  auto cloned = fragment->cloneNode(true);
  auto clonedFragment = dynamic_pointer_cast<DocumentFragment>(cloned);
  
  REQUIRE(clonedFragment != nullptr);
  REQUIRE(clonedFragment->childNodes.size() == 2);
  
  // Verify the children are cloned (different instances)
  REQUIRE(clonedFragment->childNodes[0] != fragment->childNodes[0]);
  REQUIRE(clonedFragment->childNodes[1] != fragment->childNodes[1]);
  
  // Verify the children have correct types
  REQUIRE(clonedFragment->childNodes[0]->nodeType == NodeType::TEXT_NODE);
  REQUIRE(clonedFragment->childNodes[1]->nodeType == NodeType::ELEMENT_NODE);
  
  // Verify deep cloning of nested children
  auto clonedElement = clonedFragment->childNodes[1];
  REQUIRE(clonedElement->childNodes.size() == 1);
  REQUIRE(clonedElement->childNodes[0]->nodeType == NodeType::TEXT_NODE);
  REQUIRE(clonedElement->childNodes[0] != element->childNodes[0]);
}

TEST_CASE("DocumentFragment cloneNode shallow clone does not copy children", "[DocumentFragment][cloneNode]")
{
  // Create a document and a document fragment
  auto document = make_shared<Document>();
  auto fragment = make_shared<DocumentFragment>(document);
  
  // Add a child node to the fragment
  auto textNode = make_shared<Text>("Hello", document);
  fragment->appendChild(textNode);
  
  // Verify original fragment has children
  REQUIRE(fragment->childNodes.size() == 1);
  
  // Clone the fragment with deep=false (shallow clone)
  auto cloned = fragment->cloneNode(false);
  auto clonedFragment = dynamic_pointer_cast<DocumentFragment>(cloned);
  
  REQUIRE(clonedFragment != nullptr);
  // Shallow clone should not copy children
  REQUIRE(clonedFragment->childNodes.size() == 0);
}

TEST_CASE("DocumentFragment cloned fragment maintains DocumentFragment functionality", "[DocumentFragment][cloneNode]")
{
  // Create a document and a document fragment
  auto document = make_shared<Document>();
  auto fragment = make_shared<DocumentFragment>(document);
  
  // Add some element children
  auto div1 = Element::CreateElement("div", document, false);
  auto div2 = Element::CreateElement("div", document, false);
  fragment->appendChild(div1);
  fragment->appendChild(div2);
  
  // Clone the fragment
  auto cloned = fragment->cloneNode(true);
  auto clonedFragment = dynamic_pointer_cast<DocumentFragment>(cloned);
  
  REQUIRE(clonedFragment != nullptr);
  
  // Test DocumentFragment-specific functionality
  REQUIRE(clonedFragment->childElementCount() == 2);
  REQUIRE(clonedFragment->firstElementChild() != nullptr);
  REQUIRE(clonedFragment->lastElementChild() != nullptr);
  
  auto children = clonedFragment->children();
  REQUIRE(children.size() == 2);
}