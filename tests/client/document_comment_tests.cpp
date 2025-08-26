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