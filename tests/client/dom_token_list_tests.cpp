#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <client/dom/dom_token_list.hpp>

using namespace dom;

TEST_CASE("DOMTokenList constructor", "[DOMTokenList]")
{
  DOMTokenList list1;
  REQUIRE(list1.length() == 0);
  REQUIRE(list1.value() == "");

  DOMTokenList list2("token1 token2 token3");
  REQUIRE(list2.length() == 3);
  REQUIRE(list2.value() == "token1 token2 token3");

  std::vector<std::string> supportedTokens = {"token1", "token2", "token3"};
  DOMTokenList list3("token1 token2 token3", supportedTokens);
  REQUIRE(list3.length() == 3);
  REQUIRE(list3.value() == "token1 token2 token3");
}

TEST_CASE("DOMTokenList length", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  REQUIRE(list.length() == 3);
}

TEST_CASE("DOMTokenList value", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  REQUIRE(list.value() == "token1 token2 token3");
}

TEST_CASE("DOMTokenList item", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  REQUIRE(list.item(0).value() == "token1");
  REQUIRE(list.item(1).value() == "token2");
  REQUIRE(list.item(2).value() == "token3");
  REQUIRE(list.item(3) == std::nullopt);
}

TEST_CASE("DOMTokenList contains", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  REQUIRE(list.contains("token1"));
  REQUIRE(list.contains("token2"));
  REQUIRE(list.contains("token3"));
  REQUIRE(!list.contains("token4"));
}

TEST_CASE("DOMTokenList add", "[DOMTokenList]")
{
  bool updated = false;
  DOMTokenList list("token1 token2", {}, [&](const DOMTokenList &list)
                    { updated = true; });
  list.add("token3");
  REQUIRE(list.length() == 3);
  REQUIRE(list.contains("token3"));
  REQUIRE(updated == true);
}

TEST_CASE("DOMTokenList remove", "[DOMTokenList]")
{
  bool updated = false;
  DOMTokenList list("token1 token2 token3", {}, [&](const DOMTokenList &list)
                    { updated = true; });
  list.remove("token2");
  REQUIRE(list.length() == 2);
  REQUIRE(!list.contains("token2"));
  REQUIRE(updated == true);
}

TEST_CASE("DOMTokenList replace", "[DOMTokenList]")
{
  bool updated = false;
  DOMTokenList list("token1 token2 token3", {}, [&](const DOMTokenList &list)
                    { updated = true; });
  REQUIRE(list.replace("token2", "token4"));
  REQUIRE(list.length() == 3);
  REQUIRE(!list.contains("token2"));
  REQUIRE(list.contains("token4"));
  REQUIRE(updated == true);
}

TEST_CASE("DOMTokenList supports", "[DOMTokenList]")
{
  std::vector<std::string> supportedTokens = {"token1", "token2", "token3"};
  DOMTokenList list("token1 token2 token3", supportedTokens);
  REQUIRE(list.supports("token1"));
  REQUIRE(list.supports("token2"));
  REQUIRE(list.supports("token3"));
  REQUIRE(!list.supports("token4"));
}

TEST_CASE("DOMTokenList toggle", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2");
  REQUIRE(list.toggle("token3"));
  REQUIRE(list.contains("token3"));
  REQUIRE(!list.toggle("token3"));
  REQUIRE(!list.contains("token3"));
}

TEST_CASE("DOMTokenList forEach", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  std::vector<std::string> tokens;
  list.forEach([&tokens](const std::string &token, size_t index, const DOMTokenList &list)
               { tokens.push_back(token); });
  REQUIRE(tokens.size() == 3);
  REQUIRE(tokens[0] == "token1");
  REQUIRE(tokens[1] == "token2");
  REQUIRE(tokens[2] == "token3");
}

TEST_CASE("DOMTokenList keys", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  std::vector<uint32_t> keys = list.keys();
  REQUIRE(keys.size() == 3);
  REQUIRE(keys[0] == 0);
  REQUIRE(keys[1] == 1);
  REQUIRE(keys[2] == 2);
}

TEST_CASE("DOMTokenList values", "[DOMTokenList]")
{
  DOMTokenList list("token1 token2 token3");
  std::vector<std::string> values = list.values();
  REQUIRE(values.size() == 3);
  REQUIRE(values[0] == "token1");
  REQUIRE(values[1] == "token2");
  REQUIRE(values[2] == "token3");
}
