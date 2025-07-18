#define CATCH_CONFIG_MAIN
#include "../catch2/catch_amalgamated.hpp"
#include <string>

using namespace std;

// Forward declaration of the function we want to test
void fixUnquotedAttributes(string &source);

// Include the implementation (we could also make this a static method in a testable class)
void fixUnquotedAttributes(string &source)
{
    size_t pos = 0;
    
    while (pos < source.length())
    {
        // Find the start of a tag
        size_t tagStart = source.find('<', pos);
        if (tagStart == string::npos) break;
        
        // Skip if it's a closing tag or comment
        if (tagStart + 1 < source.length() && 
            (source[tagStart + 1] == '/' || source[tagStart + 1] == '!' || source[tagStart + 1] == '?'))
        {
            pos = tagStart + 1;
            continue;
        }
        
        // Find the end of the tag
        size_t tagEnd = source.find('>', tagStart);
        if (tagEnd == string::npos) break;
        
        // Check if this is a self-closing tag
        bool isSelfClosing = tagEnd > tagStart && source[tagEnd - 1] == '/';
        
        // Process attributes within this tag
        size_t attrPos = tagStart + 1;
        
        // Skip the tag name
        while (attrPos < tagEnd && !isspace(source[attrPos])) attrPos++;
        
        while (attrPos < tagEnd)
        {
            // Skip whitespace
            while (attrPos < tagEnd && isspace(source[attrPos])) attrPos++;
            if (attrPos >= tagEnd) break;
            
            // Skip if we hit the end of a self-closing tag
            if (source[attrPos] == '/') break;
            
            // Find attribute name
            size_t attrNameStart = attrPos;
            while (attrPos < tagEnd && source[attrPos] != '=' && !isspace(source[attrPos]) && source[attrPos] != '/') attrPos++;
            
            if (attrPos >= tagEnd || source[attrPos] != '=') continue;
            
            // Skip the '=' character
            attrPos++;
            
            // Skip whitespace after '='
            while (attrPos < tagEnd && isspace(source[attrPos])) attrPos++;
            if (attrPos >= tagEnd) break;
            
            // Check if the value is already quoted
            if (source[attrPos] == '"' || source[attrPos] == '\'')
            {
                // Already quoted, find the closing quote
                char quote = source[attrPos];
                attrPos++;
                while (attrPos < tagEnd && source[attrPos] != quote) attrPos++;
                if (attrPos < tagEnd) attrPos++; // Skip closing quote
            }
            else
            {
                // Unquoted value, find the end and add quotes
                size_t valueStart = attrPos;
                while (attrPos < tagEnd && !isspace(source[attrPos]) && source[attrPos] != '/') attrPos++;
                
                if (valueStart < attrPos)
                {
                    // Insert quotes around the unquoted value
                    source.insert(attrPos, "\"");
                    source.insert(valueStart, "\"");
                    attrPos += 2; // Adjust position for the inserted quotes
                    tagEnd += 2; // Adjust tag end position
                }
            }
        }
        
        pos = tagEnd + 1;
    }
}

TEST_CASE("fixUnquotedAttributes function", "[HTML][Parsing]")
{
  SECTION("Basic unquoted attribute")
  {
    string html = "<a href=foobar></a>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<a href=\"foobar\"></a>");
  }

  SECTION("Multiple unquoted attributes")
  {
    string html = "<div class=container id=main></div>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<div class=\"container\" id=\"main\"></div>");
  }

  SECTION("Mixed quoted and unquoted attributes")
  {
    string html = "<a href=foobar title=\"quoted title\" class='single-quoted'></a>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<a href=\"foobar\" title=\"quoted title\" class='single-quoted'></a>");
  }

  SECTION("Self-closing tag with unquoted attributes")
  {
    string html = "<input type=text name=username />";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<input type=\"text\" name=\"username\" />");
  }

  SECTION("Boolean attributes remain unchanged")
  {
    string html = "<input type=checkbox checked>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<input type=\"checkbox\" checked>");
  }

  SECTION("Already quoted attributes remain unchanged")
  {
    string html = "<img src=\"image.jpg\" alt=\"test image\">";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<img src=\"image.jpg\" alt=\"test image\">");
  }

  SECTION("Complex nested HTML")
  {
    string html = "<div class=wrapper><p style=font-size:14px>Text</p></div>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<div class=\"wrapper\"><p style=\"font-size:14px\">Text</p></div>");
  }

  SECTION("Preserves spacing")
  {
    string html = "<a    href=foobar    class=test   ></a>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<a    href=\"foobar\"    class=\"test\"   ></a>");
  }

  SECTION("Ignores comments and closing tags")
  {
    string html = "<!-- comment --><div></div><a href=test>link</a>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<!-- comment --><div></div><a href=\"test\">link</a>");
  }

  SECTION("Empty string")
  {
    string html = "";
    fixUnquotedAttributes(html);
    REQUIRE(html == "");
  }

  SECTION("No attributes")
  {
    string html = "<div>content</div>";
    fixUnquotedAttributes(html);
    REQUIRE(html == "<div>content</div>");
  }
}