#include "css_selector_parser.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <regex>

namespace client_cssom::selectors
{
  using namespace std;

  // Component implementation
  Component::Component(ComponentType type, const string &name, Combinator combinator, PseudoClassType pseudoClassType)
      : type_(type)
      , name_(name)
      , combinator_(combinator)
      , pseudoClassType_(pseudoClassType)
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
    case ComponentType::kID:
      ss << "#" << name_;
      break;
    case ComponentType::kClass:
      ss << "." << name_;
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

    return nullopt;
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
}