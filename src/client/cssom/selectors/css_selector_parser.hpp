#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace endor
{
  namespace client_cssom::selectors
  {
    // Forward declarations
    class Component;
    class Selector;
    class SelectorList;

    /**
   * CSS Selector Component Types
   * These correspond to the Rust enum definitions in css_parser.rs
   */
    enum class ComponentType
    {
      kLocalName,     // Tag name (e.g., div, p, h1)
      kID,            // ID selector (e.g., #myid)
      kClass,         // Class selector (e.g., .myclass)
      kUniversal,     // Universal selector (*)
      kAttribute,     // Attribute selector (e.g., [attr], [attr=value])
      kRoot,          // :root pseudo-class
      kEmpty,         // :empty pseudo-class
      kScope,         // :scope pseudo-class
      kHost,          // :host pseudo-class
      kPseudoElement, // Pseudo-elements (e.g., ::before, ::after)
      kPseudoClass,   // Other pseudo-classes (e.g., :hover, :focus)
      kCombinator,    // Combinators (>, +, ~, space)
      kUnknown
    };

    /**
   * CSS Attribute Selector Match Types
   * These define how attribute values are matched
   */
    enum class AttributeMatchType
    {
      kExists,     // [attr] - attribute exists
      kExact,      // [attr=value] - exact match
      kWhitespace, // [attr~=value] - whitespace-separated list contains value
      kPrefix,     // [attr^=value] - starts with value
      kSuffix,     // [attr$=value] - ends with value
      kSubstring,  // [attr*=value] - contains value as substring
      kDashPrefix, // [attr|=value] - equals value or starts with value-
      kUnknown
    };

    /**
   * CSS Selector Combinators
   * These define relationships between selectors
   */
    enum class Combinator
    {
      kChild,          // > (direct child)
      kDescendant,     // space (descendant)
      kNextSibling,    // + (adjacent sibling)
      kLaterSibling,   // ~ (general sibling)
      kPseudoElement,  // :: (pseudo-element)
      kSlotAssignment, // / (slot assignment)
      kPart,           // part pseudo-element
      kUnknown
    };

    /**
   * CSS Pseudo-Class Types
   * Specific types of pseudo-classes
   */
    enum class PseudoClassType
    {
      kHover,
      kActive,
      kFocus,
      kFocusVisible,
      kFocusWithin,
      kFirstChild,
      kLastChild,
      kNthChild,
      kNthLastChild,
      kFirstOfType,
      kLastOfType,
      kNthOfType,
      kNthLastOfType,
      kOnlyChild,
      kOnlyOfType,
      kWhere,
      kUnknown
    };

    /**
   * A single component of a CSS selector
   * Can represent tag names, classes, IDs, pseudo-classes, or combinators
   */
    class Component
    {
    public:
      Component(ComponentType type, const std::string &name = "", Combinator combinator = Combinator::kUnknown, PseudoClassType pseudoClassType = PseudoClassType::kUnknown);

      // Constructor for functional pseudo-classes like :where()
      Component(ComponentType type, PseudoClassType pseudoClassType, std::shared_ptr<SelectorList> argumentSelectorList);

      // Constructor for attribute selectors
      Component(ComponentType type, const std::string &attributeName, AttributeMatchType matchType, const std::string &attributeValue = "");

      // Constructor for nth-child/nth-of-type pseudo-classes
      Component(ComponentType type, PseudoClassType pseudoClassType, int nthA, int nthB);

      // Type checking methods
      bool isLocalName() const
      {
        return type_ == ComponentType::kLocalName;
      }
      bool isId() const
      {
        return type_ == ComponentType::kID;
      }
      bool isClass() const
      {
        return type_ == ComponentType::kClass;
      }
      bool isUniversal() const
      {
        return type_ == ComponentType::kUniversal;
      }
      bool isAttribute() const
      {
        return type_ == ComponentType::kAttribute;
      }
      bool isRoot() const
      {
        return type_ == ComponentType::kRoot;
      }
      bool isEmpty() const
      {
        return type_ == ComponentType::kEmpty;
      }
      bool isHost() const
      {
        return type_ == ComponentType::kHost;
      }
      bool isPseudoElement() const
      {
        return type_ == ComponentType::kPseudoElement;
      }
      bool isPseudoClass() const
      {
        return type_ == ComponentType::kPseudoClass;
      }
      bool isCombinator() const
      {
        return type_ == ComponentType::kCombinator;
      }

      // Pseudo-class subtype checking
      bool isHover() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kHover;
      }
      bool isActive() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kActive;
      }
      bool isFocus() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kFocus;
      }
      bool isFirstChild() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kFirstChild;
      }
      bool isLastChild() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kLastChild;
      }
      bool isFirstOfType() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kFirstOfType;
      }
      bool isLastOfType() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kLastOfType;
      }
      bool isWhere() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kWhere;
      }
      bool isNthChild() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kNthChild;
      }
      bool isNthOfType() const
      {
        return isPseudoClass() && pseudoClassType_ == PseudoClassType::kNthOfType;
      }

      // Accessors
      ComponentType type() const
      {
        return type_;
      }
      const std::string &name() const
      {
        return name_;
      }
      const std::string &id() const
      {
        return name_;
      } // For ID components
      const std::string &className() const
      {
        return name_;
      } // For class components
      Combinator combinator() const
      {
        return combinator_;
      }
      PseudoClassType pseudoClassType() const
      {
        return pseudoClassType_;
      }
      const std::shared_ptr<SelectorList> &argumentSelectorList() const
      {
        return argumentSelectorList_;
      }

      // Attribute selector accessors
      const std::string &attributeName() const
      {
        return name_; // For attribute components, name_ holds the attribute name
      }
      const std::string &attributeValue() const
      {
        return attributeValue_;
      }
      AttributeMatchType attributeMatchType() const
      {
        return attributeMatchType_;
      }

      // nth-child/nth-of-type accessors (for an+b formula)
      int nthA() const
      {
        return nthA_;
      }
      int nthB() const
      {
        return nthB_;
      }

      // String representation
      operator std::string() const;

    private:
      ComponentType type_;
      std::string name_;
      Combinator combinator_;
      PseudoClassType pseudoClassType_;
      std::shared_ptr<SelectorList> argumentSelectorList_; // For functional pseudo-classes like :where()

      // Attribute selector specific fields
      AttributeMatchType attributeMatchType_;
      std::string attributeValue_;

      // nth-child/nth-of-type specific fields (for an+b formula)
      int nthA_; // 'a' coefficient in an+b
      int nthB_; // 'b' constant in an+b
    };

    /**
   * A CSS selector consists of one or more components
   * e.g., "div.class > p:hover" has multiple components
   */
    class Selector
    {
    public:
      Selector() = default;
      explicit Selector(std::vector<Component> components);

      const std::vector<Component> &components() const
      {
        return components_;
      }
      void addComponent(const Component &component)
      {
        components_.push_back(component);
      }
      bool empty() const
      {
        return components_.empty();
      }
      size_t size() const
      {
        return components_.size();
      }

      // String representation
      operator std::string() const;

    private:
      std::vector<Component> components_;
    };

    /**
   * A list of CSS selectors separated by commas
   * e.g., "div, .class, #id" is a selector list with 3 selectors
   */
    class SelectorList
    {
    public:
      SelectorList() = default;
      explicit SelectorList(std::vector<Selector> selectors);

      const std::vector<Selector> &selectors() const
      {
        return selectors_;
      }
      void addSelector(const Selector &selector)
      {
        selectors_.push_back(selector);
      }
      bool empty() const
      {
        return selectors_.empty();
      }
      size_t size() const
      {
        return selectors_.size();
      }

      // Iterator support for range-based loops
      auto begin() const
      {
        return selectors_.begin();
      }
      auto end() const
      {
        return selectors_.end();
      }
      auto begin()
      {
        return selectors_.begin();
      }
      auto end()
      {
        return selectors_.end();
      }

      // String representation
      operator std::string() const;

    private:
      std::vector<Selector> selectors_;
    };

    /**
   * CSS Selector Parser
   * Parses CSS selector strings into structured selector lists
   */
    class CSSelectorParser
    {
    public:
      /**
     * Parse a CSS selector string into a SelectorList
     * @param selectorText The CSS selector string to parse
     * @return Optional SelectorList if parsing succeeds, nullopt otherwise
     */
      static std::optional<SelectorList> parseSelectors(const std::string &selectorText);

    private:
      // Internal parsing methods
      static std::optional<std::vector<Selector>> parseMultipleSelectors(const std::string &text);
      static std::optional<Selector> parseSingleSelector(const std::string &text);
      static std::optional<Component> parseComponent(const std::string &text, size_t &pos);
      static std::optional<Component> parseFunctionalPseudoClass(const std::string &name, const std::string &text, size_t &pos);
      static std::optional<Component> parseAttributeSelector(const std::string &text, size_t &pos);
      static std::optional<Combinator> parseCombinator(const std::string &text, size_t &pos);
      static std::optional<PseudoClassType> parsePseudoClass(const std::string &name);

      // Utility methods
      static void skipWhitespace(const std::string &text, size_t &pos);
      static std::string parseIdentifier(const std::string &text, size_t &pos);
      static bool isIdentifierStart(char c);
      static bool isIdentifierChar(char c);
      static bool parseNthFormula(const std::string &formula, int &a, int &b);
    };
  }
} // namespace endor