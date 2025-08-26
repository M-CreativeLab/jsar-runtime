#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/document.hpp>
#include <client/dom/comment.hpp>
#include <client/dom/browsing_context.hpp>

using namespace dom;

TEST_CASE("Document createComment", "[Document][Comment]")
{
  auto browsingContext = std::make_shared<BrowsingContext>();
  auto document = std::make_shared<HTMLDocument>(browsingContext, false);

  SECTION("creates comment with text")
  {
    auto comment = document->createComment("test comment");
    REQUIRE(comment != nullptr);
    REQUIRE(comment->nodeType == NodeType::COMMENT_NODE);
    REQUIRE(comment->data() == "test comment");
    REQUIRE(comment->ownerDocument == document);
  }

  SECTION("creates comment with empty text")
  {
    auto comment = document->createComment("");
    REQUIRE(comment != nullptr);
    REQUIRE(comment->nodeType == NodeType::COMMENT_NODE);
    REQUIRE(comment->data() == "");
    REQUIRE(comment->ownerDocument == document);
  }

  SECTION("creates comment with special characters")
  {
    auto comment = document->createComment("<!-- test with HTML-like content -->");
    REQUIRE(comment != nullptr);
    REQUIRE(comment->nodeType == NodeType::COMMENT_NODE);
    REQUIRE(comment->data() == "<!-- test with HTML-like content -->");
    REQUIRE(comment->ownerDocument == document);
  }
}

TEST_CASE("Comment node properties", "[Comment]")
{
  auto browsingContext = std::make_shared<BrowsingContext>();
  auto document = std::make_shared<HTMLDocument>(browsingContext, false);
  auto comment = document->createComment("test comment");

  SECTION("node name is correct")
  {
    REQUIRE(comment->nodeName == "#comment");
  }

  SECTION("node type is comment")
  {
    REQUIRE(comment->nodeType == NodeType::COMMENT_NODE);
  }

  SECTION("text content matches data")
  {
    REQUIRE(comment->textContent == "test comment");
  }
}

TEST_CASE("Comment DOM tree integration", "[Comment][DOM]")
{
  auto browsingContext = std::make_shared<BrowsingContext>();
  auto document = std::make_shared<HTMLDocument>(browsingContext, false);
  
  // Set up a basic document structure
  document->setSource("<html><body></body></html>");
  auto body = document->body();
  REQUIRE(body != nullptr);

  SECTION("comment can be appended to body")
  {
    auto comment = document->createComment("test comment");
    body->appendChild(comment);
    
    REQUIRE(comment->parentNode.lock() == body);
    REQUIRE(body->childNodes.size() == 1);
    REQUIRE(body->childNodes[0] == comment);
  }

  SECTION("multiple comments can be added")
  {
    auto comment1 = document->createComment("first comment");
    auto comment2 = document->createComment("second comment");
    
    body->appendChild(comment1);
    body->appendChild(comment2);
    
    REQUIRE(body->childNodes.size() == 2);
    REQUIRE(body->childNodes[0] == comment1);
    REQUIRE(body->childNodes[1] == comment2);
  }

  SECTION("comment serialization works correctly")
  {
    auto comment = document->createComment("test comment");
    body->appendChild(comment);
    
    std::string serialized = HTMLDocument::SerializeFragment(body);
    REQUIRE(serialized.find("<!--test comment-->") != std::string::npos);
  }

  SECTION("empty comment serialization")
  {
    auto comment = document->createComment("");
    body->appendChild(comment);
    
    std::string serialized = HTMLDocument::SerializeFragment(body);
    REQUIRE(serialized.find("<!---->") != std::string::npos);
  }

  SECTION("comment with special characters serialization")
  {
    auto comment = document->createComment("special & chars < > \" '");
    body->appendChild(comment);
    
    std::string serialized = HTMLDocument::SerializeFragment(body);
    REQUIRE(serialized.find("<!--special & chars < > \" '-->") != std::string::npos);
  }
}