# CSS Selector Parser Implementation

This document describes the implementation of the native C++ CSS selector parser for JSAR Runtime, which replaces the Rust-based selector parsing system with a pure C++ implementation.

## Overview

The new CSS selector parser is implemented in `src/client/cssom/selectors/` and provides:
- Full CSS selector parsing in native C++
- Support for all standard CSS selector types
- Integration with the existing selector matching system
- Performance optimization for large stylesheets

## Architecture

### Core Components

1. **CSSelectorParser** (`css_selector_parser.hpp/cpp`)
   - Main parser class with static methods
   - Handles tokenization and parsing of CSS selector strings
   - Returns structured `SelectorList` objects

2. **Data Structures**
   - `Component`: Represents individual selector parts (tag, class, ID, pseudo-class, combinator)
   - `Selector`: Collection of components forming a complete selector
   - `SelectorList`: Collection of selectors (comma-separated)

3. **Matching System** (`matching.hpp/cpp`)
   - Updated to support both Rust and native C++ selectors
   - Overloaded functions for compatibility
   - Enhanced with additional pseudo-class support

4. **Integration** (`css_style_rule.hpp`)
   - Updated `CSSStyleRule` to use C++ parser
   - Fallback to Rust parser for compatibility
   - Added utility methods for parser selection

## Supported CSS Selectors

### Basic Selectors
- **Type selectors**: `div`, `p`, `span`
- **Class selectors**: `.myclass`, `.nav-item`
- **ID selectors**: `#myid`, `#header`

### Pseudo-classes
- `:hover`, `:active`, `:focus`
- `:focus-visible`, `:focus-within`
- `:first-child`, `:last-child`
- `:first-of-type`, `:last-of-type`
- `:only-child`, `:only-of-type`
- `:nth-child()`, `:nth-of-type()` (supports `odd`, `even`, numbers, and `an+b` formulas)
- `:root`, `:empty`, `:host`

### Pseudo-elements
- `::before`, `::after`
- `::first-line`, `::first-letter`

### Combinators
- **Descendant**: `div p` (space)
- **Child**: `div > p`
- **Adjacent sibling**: `h1 + p`
- **General sibling**: `h1 ~ p`

### Complex Selectors
- **Compound**: `div.class#id:hover`
- **Multiple**: `body, .foo > div#bar`
- **Nested**: `.container > .item:first-child`

## Usage Examples

### Basic Parsing
```cpp
#include <client/cssom/selectors/css_selector_parser.hpp>

using namespace client_cssom::selectors;

// Parse a simple selector
auto result = CSSelectorParser::parseSelectors("div.class");
if (result) {
    const auto& selectorList = *result;
    // Use the parsed selectors
}
```

### Integration with Style Rules
```cpp
#include <client/cssom/rules/css_style_rule.hpp>

// The updated CSSStyleRule automatically uses C++ parser
CSSStyleRule rule(rustStyleRule);

// Check which parser is being used
if (rule.usesNativeParser()) {
    // Using C++ parser
    const auto* nativeSelectors = rule.nativeSelectors();
    // Work with native selectors
}

// Match elements
bool matches = rule.matches(htmlElement);
```

### Selector Matching
```cpp
#include <client/cssom/selectors/matching.hpp>

// Parse selectors
auto selectors = CSSelectorParser::parseSelectors("div > p:hover");

// Match against element
bool matches = matchesSelectorList(*selectors, element);
```

### nth-child Selector Examples
```cpp
// Parse various nth-child patterns
auto oddSelector = CSSelectorParser::parseSelectors("li:nth-child(odd)");
auto evenSelector = CSSelectorParser::parseSelectors("tr:nth-child(even)");
auto specificSelector = CSSelectorParser::parseSelectors("div:nth-child(3)");
auto formulaSelector = CSSelectorParser::parseSelectors("p:nth-child(3n+2)");
auto negativeSelector = CSSelectorParser::parseSelectors("span:nth-child(-n+3)");

// Access nth-child parameters
auto component = oddSelector->selectors()[0].components()[1];
assert(component.isNthChild());
assert(component.nthA() == 2);  // 'a' coefficient in 2n+1
assert(component.nthB() == 1);  // 'b' constant in 2n+1

// String representation preserves original format
std::string str = static_cast<std::string>(*oddSelector);
assert(str == "li:nth-child(odd)");
```

## Performance Characteristics

### Parser Performance
- **Tokenization**: O(n) where n is the selector string length
- **Parsing**: O(m) where m is the number of components
- **Memory**: Minimal allocations, move semantics used throughout

### Matching Performance
- **Simple selectors**: O(1) for most cases
- **Complex selectors**: O(d) where d is DOM depth for descendant combinators
- **Selector lists**: O(s×c) where s is selectors count, c is component cost

## Implementation Details

### Error Handling
- Invalid selectors return `std::nullopt`
- Graceful fallback to Rust parser when C++ parsing fails
- No exceptions thrown during normal operation

### Memory Management
- RAII principles throughout
- Move semantics for large objects
- Minimal dynamic allocation

### Thread Safety
- Parser is stateless and thread-safe
- No global state or mutable static data
- Safe for concurrent use across threads

## Migration Guide

### From Rust to C++ Parser

The migration is automatic and transparent:

1. **Existing Code**: No changes required
2. **New Features**: Automatically use C++ parser
3. **Fallback**: Rust parser used if C++ parsing fails

### Testing Migration

```cpp
// Old way (Rust-based)
auto rustSelectors = parseSelectorsRust("div.class");

// New way (C++ native)
auto cppSelectors = CSSelectorParser::parseSelectors("div.class");

// Both produce compatible results for matching
```

## Testing

### Unit Tests
Located in `tests/client/css_selector_parser_tests.cpp`:
- Basic selector parsing
- Complex selector combinations
- Error cases and edge conditions
- Performance benchmarks

### Integration Tests
- Compatibility with existing CSSOM
- Matching accuracy vs. Rust implementation
- Style application correctness

### Running Tests
```bash
# Format check
./tools/clang-format-check.sh

# Lint TypeScript
npm run lint

# Full test suite (macOS only)
make test
```

## Future Enhancements

### Planned Features
1. **Advanced Pseudo-classes**
   - `:not()`, `:is()`, `:where()`
   - `:has()` (if CSS4 support needed)

2. **Performance Optimizations**
   - Selector compilation/caching
   - Fast-path matching for common patterns
   - SIMD optimizations for string matching

3. **Additional Selectors**
   - Attribute selectors with operators
   - Case-insensitive matching
   - Language pseudo-classes

### Extension Points
- **Custom Pseudo-classes**: Add new pseudo-class types via enum extension
- **Custom Combinators**: Support for framework-specific combinators
- **Validation Hooks**: Pre/post-parsing validation callbacks

## Troubleshooting

### Common Issues

1. **Parse Failures**
   - Check selector syntax
   - Verify supported features
   - Enable Rust fallback logging

2. **Matching Discrepancies**
   - Compare with browser DevTools
   - Check pseudo-class implementations
   - Verify DOM structure assumptions

3. **Performance Issues**
   - Profile complex selectors
   - Consider selector simplification
   - Check for recursive matching patterns

### Debug Mode
```cpp
// Enable debug output (compile-time flag)
#define CSS_PARSER_DEBUG 1
#include <client/cssom/selectors/css_selector_parser.hpp>
```

## References

- [MDN CSS Selectors](https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_selectors)
- [CSS Selectors Level 4 Specification](https://www.w3.org/TR/selectors-4/)
- [JSAR Runtime Architecture](../../../docs/architecture.md)