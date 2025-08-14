# Inline Layout Support in JSAR Runtime

This document describes the implementation of `display: inline` support in the JSAR Runtime layout system.

## Problem

Previously, while the layout system had infrastructure for inline elements (LayoutInline class, InlineFormattingContext), the layout computation was not handling inline display correctly. The original implementation mapped inline elements to block layout at the taffy level, which broke standard CSS inline layout behavior.

## Architectural Solution

Following feedback from @yorkie, the implementation now uses a hybrid approach that provides true inline layout behavior while integrating with the existing taffy-based layout system.

### Key Components

1. **Custom InlineFormattingContext**: 
   - Does NOT inherit from `TaffyBasedFormattingContext`
   - Implements actual inline layout algorithms (line wrapping, baseline alignment, text flow)
   - Manages its own layout tree for inline-specific behavior

2. **Taffy Integration via Placeholder**:
   - Creates a taffy placeholder node to represent inline content in the taffy tree
   - Allows taffy-based parents (blocks, flex, grid) to account for space taken by inline content
   - Updates placeholder size based on computed inline layout

3. **Hybrid Layout Tree Management**:
   - Taffy manages block-level layout (blocks, flex, grid containers)
   - InlineFormattingContext manages inline-level layout (text flow, line boxes)
   - Seamless integration between the two systems

## Technical Implementation

### InlineFormattingContext Architecture

```cpp
class InlineFormattingContext : public FormattingContext
{
  // Custom inline layout computation
  void computeInlineLayout(const ConstraintSpace &space);
  
  // Taffy integration
  std::unique_ptr<crates::layout2::Node> taffy_placeholder_;
  void updateTaffyPlaceholder();
  
  // Inline-specific data structures
  struct LineBox { /* line layout info */ };
  std::vector<LineBox> line_boxes_;
};
```

### Layout Tree Integration

```
TaffyBasedFormattingContext (Block)
├── TaffyBasedFormattingContext (Flex)
├── InlineFormattingContext → taffy_placeholder_
│   ├── Custom inline layout computation
│   └── LineBox management
└── TaffyBasedFormattingContext (Grid)
```

### Display Type Routing

- `display: block`, `display: flex`, `display: grid` → `TaffyBasedFormattingContext`
- `display: inline`, `display: inline-block`, etc. → `InlineFormattingContext`

## CSS Standards Compliance

Now supports all CSS inline display variants:
- `display: inline` - Basic inline layout with text flow
- `display: inline-block` - Inline block formatting
- `display: inline-flex` - Inline flex containers  
- `display: inline-grid` - Inline grid containers
- `display: inline-table` - Inline table containers

## Benefits

1. **True Inline Behavior**: Text flows horizontally, wraps to new lines, proper baseline alignment
2. **CSS Compliance**: Follows CSS specification for inline layout behavior
3. **Performance**: Custom algorithms optimized for inline content vs. generic block layout
4. **Integration**: Seamless interaction with existing taffy-based block layout
5. **Extensibility**: Framework for implementing advanced inline features (bidirectional text, complex scripts)

## Future Enhancements

The current implementation provides a foundation for:
- Advanced text layout (bidirectional text, complex scripts)
- Inline-block positioning
- Baseline alignment across mixed content
- Line-height and vertical rhythm
- Text decoration and emphasis

## Testing

Comprehensive tests verify:
- CSS display value parsing for all inline variants
- DisplayType conversion and routing  
- Layout object creation pipeline
- Integration with existing block, flex, and grid layouts
- Standards compliance for common inline HTML elements

This implementation addresses the architectural requirement to handle inline layout properly without relying on taffy's block layout mapping, providing a robust foundation for CSS-compliant inline element rendering.