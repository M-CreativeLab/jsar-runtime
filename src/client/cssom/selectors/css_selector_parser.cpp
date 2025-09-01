#include <algorithm>
#include <sstream>
#include <cctype>
#include "./css_selector_parser.hpp"

namespace client_cssom::selectors
{
  using namespace std;

  // Component implementation
  Component::Component(ComponentType type, const string &name, Combinator combinator, PseudoClassType pseudoClassType)
      : type_(type)
      , name_(name)
      , combinator_(combinator)
      , pseudoClassType_(pseudoClassType)
      , argumentSelectorList_(nullptr)
      , attributeMatchType_(AttributeMatchType::kUnknown)
      , attributeValue_("")
  {
  }

  Component::Component(ComponentType type, PseudoClassType pseudoClassType, std::shared_ptr<SelectorList> argumentSelectorList)
      : type_(type)
      , name_("")
      , combinator_(Combinator::kUnknown)
      , pseudoClassType_(pseudoClassType)
      , argumentSelectorList_(argumentSelectorList)
      , attributeMatchType_(AttributeMatchType::kUnknown)
      , attributeValue_("")
  {
  }

  Component::Component(ComponentType type, const string &attributeName, AttributeMatchType matchType, const string &attributeValue)
      : type_(type)
      , name_(attributeName)
      , combinator_(Combinator::kUnknown)
      , pseudoClassType_(PseudoClassType::kUnknown)
      , argumentSelectorList_(nullptr)
      , attributeMatchType_(matchType)
      , attributeValue_(attributeValue)
  {
  }

  Component::operator string() const
  {
    stringstream ss;
    switch (type_)
    {
    case ComponentType::kLocalName:
      ss << name_;
      break;
    case ComponentType::kUniversal:
      ss << "*";
      break;
    case ComponentType::kID:
      ss << "#" << name_;
      break;
    case ComponentType::kClass:
      ss << "." << name_;
      break;
    case ComponentType::kAttribute:
      ss << "[" << name_;
      switch (attributeMatchType_)
      {
      case AttributeMatchType::kExists:
        break; // Just [attr]
      case AttributeMatchType::kExact:
        ss << "=\"" << attributeValue_ << "\"";
        break;
      case AttributeMatchType::kWhitespace:
        ss << "~=\"" << attributeValue_ << "\"";
        break;
      case AttributeMatchType::kPrefix:
        ss << "^=\"" << attributeValue_ << "\"";
        break;
      case AttributeMatchType::kSuffix:
        ss << "$=\"" << attributeValue_ << "\"";
        break;
      case AttributeMatchType::kSubstring:
        ss << "*=\"" << attributeValue_ << "\"";
        break;
      case AttributeMatchType::kDashPrefix:
        ss << "|=\"" << attributeValue_ << "\"";
        break;
      default:
        break;
      }
      ss << "]";
      break;
    case ComponentType::kRoot:
      ss << ":root";
      break;
    case ComponentType::kEmpty:
      ss << ":empty";
      break;
    case ComponentType::kHost:
      ss << ":host";
      break;
    case ComponentType::kPseudoElement:
      ss << "::" << name_;
      break;
    case ComponentType::kPseudoClass:
      switch (pseudoClassType_)
      {
      case PseudoClassType::kHover:
        ss << ":hover";
        break;
      case PseudoClassType::kActive:
        ss << ":active";
        break;
      case PseudoClassType::kFocus:
        ss << ":focus";
        break;
      case PseudoClassType::kFocusVisible:
        ss << ":focus-visible";
        break;
      case PseudoClassType::kFocusWithin:
        ss << ":focus-within";
        break;
      case PseudoClassType::kFirstChild:
        ss << ":first-child";
        break;
      case PseudoClassType::kLastChild:
        ss << ":last-child";
        break;
      case PseudoClassType::kFirstOfType:
        ss << ":first-of-type";
        break;
      case PseudoClassType::kLastOfType:
        ss << ":last-of-type";
        break;
      case PseudoClassType::kOnlyChild:
        ss << ":only-child";
        break;
      case PseudoClassType::kOnlyOfType:
        ss << ":only-of-type";
        break;
      case PseudoClassType::kWhere:
        ss << ":where(";
        if (argumentSelectorList_)
        {
          ss << static_cast<string>(*argumentSelectorList_);
        }
        ss << ")";
        break;
      default:
        ss << ":" << name_;
        break;
      }
      break;
    case ComponentType::kCombinator:
      switch (combinator_)
      {
      case Combinator::kDescendant:
        ss << " ";
        break;
      case Combinator::kChild:
        ss << " > ";
        break;
      case Combinator::kNextSibling:
        ss << " + ";
        break;
      case Combinator::kLaterSibling:
        ss << " ~ ";
        break;
      case Combinator::kPseudoElement:
        ss << "::";
        break;
      default:
        ss << " ? ";
        break;
      }
      break;
    default:
      break;
    }
    return ss.str();
  }

  // Selector implementation
  Selector::Selector(vector<Component> components)
      : components_(move(components))
  {
  }

  Selector::operator string() const
  {
    stringstream ss;
    for (size_t i = 0; i < components_.size(); ++i)
    {
      if (i > 0 && !components_[i].isCombinator())
      {
        // Add implicit space for compound selectors
        if (!components_[i - 1].isCombinator())
        {
          // This handles cases like "div.class" (no space between tag and class)
        }
      }
      ss << static_cast<string>(components_[i]);
    }
    return ss.str();
  }

  // SelectorList implementation
  SelectorList::SelectorList(vector<Selector> selectors)
      : selectors_(move(selectors))
  {
  }

  SelectorList::operator string() const
  {
    stringstream ss;
    for (size_t i = 0; i < selectors_.size(); ++i)
    {
      if (i > 0)
        ss << ", ";
      ss << static_cast<string>(selectors_[i]);
    }
    return ss.str();
  }

  // CSSelectorParser implementation
  optional<SelectorList> CSSelectorParser::parseSelectors(const string &selectorText)
  {
    auto selectors = parseMultipleSelectors(selectorText);
    if (!selectors)
      return nullopt;

    return SelectorList(move(*selectors));
  }

  optional<vector<Selector>> CSSelectorParser::parseMultipleSelectors(const string &text)
  {
    vector<Selector> selectors;
    stringstream ss(text);
    string selectorText;

    // Split by comma to get individual selectors
    while (getline(ss, selectorText, ','))
    {
      // Trim whitespace
      selectorText.erase(0, selectorText.find_first_not_of(" \t\n\r"));
      selectorText.erase(selectorText.find_last_not_of(" \t\n\r") + 1);

      if (selectorText.empty())
        continue;

      auto selector = parseSingleSelector(selectorText);
      if (!selector)
        return nullopt;

      selectors.push_back(move(*selector));
    }

    return selectors;
  }

  optional<Selector> CSSelectorParser::parseSingleSelector(const string &text)
  {
    vector<Component> components;
    size_t pos = 0;

    while (pos < text.length())
    {
      skipWhitespace(text, pos);
      if (pos >= text.length())
        break;

      // Try to parse a component first
      auto component = parseComponent(text, pos);
      if (component)
      {
        components.push_back(move(*component));

        // After parsing a component, check for combinators
        // Don't skip whitespace here - let parseCombinator handle it
        if (pos < text.length())
        {
          auto combinator = parseCombinator(text, pos);
          if (combinator)
          {
            components.emplace_back(ComponentType::kCombinator, "", *combinator);
          }
        }
        continue;
      }

      // If we can't parse a component, something is wrong
      return nullopt;
    }

    return Selector(move(components));
  }

  optional<Component> CSSelectorParser::parseComponent(const string &text, size_t &pos)
  {
    if (pos >= text.length())
      return nullopt;

    char c = text[pos];

    // Universal selector (*)
    if (c == '*')
    {
      ++pos;
      return Component(ComponentType::kUniversal);
    }

    // ID selector (#id)
    if (c == '#')
    {
      ++pos;
      auto name = parseIdentifier(text, pos);
      if (name.empty())
        return nullopt;
      return Component(ComponentType::kID, name);
    }

    // Class selector (.class)
    if (c == '.')
    {
      ++pos;
      auto name = parseIdentifier(text, pos);
      if (name.empty())
        return nullopt;
      return Component(ComponentType::kClass, name);
    }

    // Attribute selector ([attr] or [attr=value])
    if (c == '[')
    {
      return parseAttributeSelector(text, pos);
    }

    // Pseudo-class or pseudo-element (:pseudo or ::pseudo)
    if (c == ':')
    {
      ++pos;
      bool isPseudoElement = false;

      // Check for :: (pseudo-element)
      if (pos < text.length() && text[pos] == ':')
      {
        ++pos;
        isPseudoElement = true;
      }

      auto name = parseIdentifier(text, pos);
      if (name.empty())
        return nullopt;

      if (isPseudoElement)
      {
        return Component(ComponentType::kPseudoElement, name);
      }
      else
      {
        // Handle special pseudo-classes
        if (name == "root")
          return Component(ComponentType::kRoot);
        if (name == "empty")
          return Component(ComponentType::kEmpty);
        if (name == "host")
          return Component(ComponentType::kHost);

        // Check for functional pseudo-classes like :where()
        if (pos < text.length() && text[pos] == '(')
        {
          auto functionalComponent = parseFunctionalPseudoClass(name, text, pos);
          if (functionalComponent)
            return functionalComponent;
          // If functional parsing failed, fall through to regular pseudo-class parsing
        }

        // Regular pseudo-class
        auto pseudoClassType = parsePseudoClass(name);
        return Component(ComponentType::kPseudoClass, name, Combinator::kUnknown, pseudoClassType.value_or(PseudoClassType::kUnknown));
      }
    }

    // Tag name (element selector)
    if (isIdentifierStart(c))
    {
      auto name = parseIdentifier(text, pos);
      if (name.empty())
        return nullopt;
      return Component(ComponentType::kLocalName, name);
    }

    return nullopt;
  }

  optional<Combinator> CSSelectorParser::parseCombinator(const string &text, size_t &pos)
  {
    if (pos >= text.length())
      return nullopt;

    // Skip any leading whitespace to handle cases like "div > p"
    size_t originalPos = pos;
    skipWhitespace(text, pos);

    if (pos >= text.length())
      return nullopt;

    char c = text[pos];

    switch (c)
    {
    case '>':
      ++pos;
      skipWhitespace(text, pos);
      return Combinator::kChild;
    case '+':
      ++pos;
      skipWhitespace(text, pos);
      return Combinator::kNextSibling;
    case '~':
      ++pos;
      skipWhitespace(text, pos);
      return Combinator::kLaterSibling;
    }

    // Check for descendant combinator (whitespace that was skipped above)
    if (originalPos != pos)
    {
      // We skipped whitespace but didn't find an explicit combinator
      // This means it's a descendant combinator
      // Make sure there's content after the whitespace
      if (pos < text.length())
      {
        return Combinator::kDescendant;
      }
    }

    // Restore position if we didn't find any combinator
    pos = originalPos;
    return nullopt;
  }

  optional<PseudoClassType> CSSelectorParser::parsePseudoClass(const string &name)
  {
    if (name == "hover")
      return PseudoClassType::kHover;
    if (name == "active")
      return PseudoClassType::kActive;
    if (name == "focus")
      return PseudoClassType::kFocus;
    if (name == "focus-visible")
      return PseudoClassType::kFocusVisible;
    if (name == "focus-within")
      return PseudoClassType::kFocusWithin;
    if (name == "first-child")
      return PseudoClassType::kFirstChild;
    if (name == "last-child")
      return PseudoClassType::kLastChild;
    if (name == "first-of-type")
      return PseudoClassType::kFirstOfType;
    if (name == "last-of-type")
      return PseudoClassType::kLastOfType;
    if (name == "only-child")
      return PseudoClassType::kOnlyChild;
    if (name == "only-of-type")
      return PseudoClassType::kOnlyOfType;
    if (name == "where")
      return PseudoClassType::kWhere;

    return nullopt;
  }

  optional<Component> CSSelectorParser::parseFunctionalPseudoClass(const string &name, const string &text, size_t &pos)
  {
    if (name != "where")
      return nullopt; // Only :where() is supported for now

    // Expect opening parenthesis
    if (pos >= text.length() || text[pos] != '(')
      return nullopt;

    ++pos; // Skip '('

    // Find the matching closing parenthesis
    size_t startPos = pos;
    int parenCount = 1;
    size_t endPos = pos;

    while (endPos < text.length() && parenCount > 0)
    {
      if (text[endPos] == '(')
        parenCount++;
      else if (text[endPos] == ')')
        parenCount--;

      endPos++; // Always move forward
    }

    if (parenCount != 0)
      return nullopt; // Unmatched parentheses

    // endPos is now one position after the closing parenthesis
    // The content is from startPos to endPos-1 (exclusive of closing paren)
    string selectorListText = text.substr(startPos, endPos - 1 - startPos);

    // Parse the selector list
    auto argumentSelectors = parseMultipleSelectors(selectorListText);
    if (!argumentSelectors)
      return nullopt;

    // Create shared_ptr directly
    auto selectorListPtr = make_shared<SelectorList>(*argumentSelectors);

    // Update position to after the closing parenthesis
    pos = endPos;

    return Component(ComponentType::kPseudoClass, PseudoClassType::kWhere, selectorListPtr);
  }

  void CSSelectorParser::skipWhitespace(const string &text, size_t &pos)
  {
    while (pos < text.length() && isspace(text[pos]))
      ++pos;
  }

  string CSSelectorParser::parseIdentifier(const string &text, size_t &pos)
  {
    string result;

    if (pos >= text.length() || !isIdentifierStart(text[pos]))
      return result;

    while (pos < text.length() && isIdentifierChar(text[pos]))
    {
      result += text[pos];
      ++pos;
    }

    return result;
  }

  bool CSSelectorParser::isIdentifierStart(char c)
  {
    return isalpha(c) || c == '_' || c == '-';
  }

  bool CSSelectorParser::isIdentifierChar(char c)
  {
    return isalnum(c) || c == '_' || c == '-';
  }

  optional<Component> CSSelectorParser::parseAttributeSelector(const string &text, size_t &pos)
  {
    if (pos >= text.length() || text[pos] != '[')
      return nullopt;

    ++pos; // Skip '['

    skipWhitespace(text, pos);

    // Parse attribute name
    auto attributeName = parseIdentifier(text, pos);
    if (attributeName.empty())
      return nullopt;

    skipWhitespace(text, pos);

    // Check if it's just [attr] (existence check)
    if (pos < text.length() && text[pos] == ']')
    {
      ++pos;
      return Component(ComponentType::kAttribute, attributeName, AttributeMatchType::kExists);
    }

    // Parse attribute match operator
    AttributeMatchType matchType = AttributeMatchType::kUnknown;
    if (pos < text.length())
    {
      char c = text[pos];
      if (c == '=')
      {
        matchType = AttributeMatchType::kExact;
        ++pos;
      }
      else if (pos + 1 < text.length() && text[pos + 1] == '=')
      {
        switch (c)
        {
        case '~':
          matchType = AttributeMatchType::kWhitespace;
          break;
        case '^':
          matchType = AttributeMatchType::kPrefix;
          break;
        case '$':
          matchType = AttributeMatchType::kSuffix;
          break;
        case '*':
          matchType = AttributeMatchType::kSubstring;
          break;
        case '|':
          matchType = AttributeMatchType::kDashPrefix;
          break;
        default:
          return nullopt;
        }
        pos += 2; // Skip operator and '='
      }
      else
      {
        return nullopt; // Invalid operator
      }
    }
    else
    {
      return nullopt; // Unexpected end
    }

    skipWhitespace(text, pos);

    // Parse attribute value
    string attributeValue;
    if (pos < text.length())
    {
      char c = text[pos];
      if (c == '"' || c == '\'')
      {
        // Quoted string
        char quote = c;
        ++pos;
        while (pos < text.length() && text[pos] != quote)
        {
          if (text[pos] == '\\' && pos + 1 < text.length())
          {
            // Handle escape sequences
            ++pos;
            if (pos < text.length())
            {
              attributeValue += text[pos];
              ++pos;
            }
          }
          else
          {
            attributeValue += text[pos];
            ++pos;
          }
        }
        if (pos < text.length() && text[pos] == quote)
          ++pos; // Skip closing quote
        else
          return nullopt; // Unclosed string
      }
      else
      {
        // Unquoted identifier
        attributeValue = parseIdentifier(text, pos);
        if (attributeValue.empty())
          return nullopt;
      }
    }

    skipWhitespace(text, pos);

    // Expect closing ']'
    if (pos >= text.length() || text[pos] != ']')
      return nullopt;
    ++pos;

    return Component(ComponentType::kAttribute, attributeName, matchType, attributeValue);
  }
}