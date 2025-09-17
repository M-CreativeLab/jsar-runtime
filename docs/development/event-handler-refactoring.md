# Element Event Handler Refactoring

## Overview

This document describes the refactoring of `Element::executeOnClickHandler()` to a generalized `executeEventHandler(type, event)` method, enabling support for multiple event types while maintaining backward compatibility.

## Background

Previously, the Element class only supported onclick event handlers through:
- `executeOnClickHandler()` - executed handler code without event objects
- `setOnClickHandler(code)` - set onclick handler code
- `hasOnClickHandler()` - check if onclick handler exists

This limited approach prevented support for other important event types like `pointerdown`, `mouseover`, `keydown`, etc.

## New API

### Core Method

```cpp
void executeEventHandler(const std::string &type, dom::Event *event)
```

**Parameters:**
- `type`: Event type string (e.g., "click", "pointerdown", "mouseover")
- `event`: Pointer to the event object (can be nullptr for backward compatibility)

**Behavior:**
- Locates the appropriate event handler code or function reference for the given type
- Executes the handler with the event object passed as the first parameter
- Handles both inline code strings and function references
- Gracefully handles missing handlers or execution errors

### Supporting Methods

```cpp
// Set handler code for any event type
void setEventHandler(const std::string &type, const std::string &handlerCode)

// Set function reference for any event type  
void setEventHandlerFunction(const std::string &type, void *functionRef)

// Get handler code for any event type
std::string getEventHandlerCode(const std::string &type) const

// Check if handler exists for any event type
bool hasEventHandler(const std::string &type) const
```

## Implementation Details

### Storage

The implementation uses map-based storage for multiple event types:

```cpp
// Generalized storage for multiple event types
std::unordered_map<std::string, std::string> event_handler_codes_;
std::unordered_map<std::string, void*> event_handler_function_refs_;

// Legacy onclick storage maintained for backward compatibility
std::string onclick_handler_code_;
bool has_onclick_function_ = false;
void *onclick_function_ref_ = nullptr;
```

### Event Object Passing

Key improvement: Event objects are now passed to handlers:

```cpp
// Before: Empty event object
string scriptCode = "(function(event) { " + onclick_handler_code_ + " })({});";

// After: Actual event object or null
string eventParam = event ? "event" : "null";
string scriptCode = "(function(event) { " + handlerCode + " })(" + eventParam + ");";
```

### Integration Points

The new system integrates seamlessly with existing event simulation:

```cpp
void Element::simulateClick(const glm::vec3 &hitPointInWorld)
{
  // Create a click event object to pass to the handler
  auto clickEvent = events::PointerEvent::Click();
  
  // Execute click handler if it exists, passing the event object
  if (hasEventHandler("click"))
    executeEventHandler("click", clickEvent.get());

  dispatchEventInternal(std::move(clickEvent));
}
```

## Backward Compatibility

All existing onclick methods remain functional and delegate to the new system:

```cpp
void Element::setOnClickHandler(const std::string &handlerCode)
{
  setEventHandler("click", handlerCode);
}

bool Element::hasOnClickHandler() const
{
  return hasEventHandler("click");
}

void Element::executeOnClickHandler()
{
  executeEventHandler("click", nullptr);
}
```

This ensures existing code continues to work without modification.

## Usage Examples

### Basic Usage

```cpp
auto element = std::make_shared<Element>("button", std::nullopt);

// Set handlers for multiple event types
element->setEventHandler("click", "handleClick(event);");
element->setEventHandler("pointerdown", "event.preventDefault(); startDrag(event);");
element->setEventHandler("mouseover", "this.style.backgroundColor = 'lightblue';");

// Check if handlers exist
if (element->hasEventHandler("click")) {
    // Execute with event object
    auto clickEvent = events::PointerEvent::Click();
    element->executeEventHandler("click", clickEvent.get());
}
```

### Legacy Compatibility

```cpp
// Existing code continues to work
element->setOnClickHandler("alert('clicked')");
if (element->hasOnClickHandler()) {
    element->executeOnClickHandler();
}

// But can also be accessed through new API
if (element->hasEventHandler("click")) {
    auto code = element->getEventHandlerCode("click");
    // code == "alert('clicked')"
}
```

### Real-World Scenarios

```cpp
// Interactive UI with touch/pointer support
element->setEventHandler("pointerdown", "startInteraction(event);");
element->setEventHandler("pointerup", "endInteraction(event);");
element->setEventHandler("pointermove", "updatePosition(event);");

// Form validation
input->setEventHandler("focus", "highlightField(this);");
input->setEventHandler("blur", "validateField(this);");
input->setEventHandler("keydown", "if (event.key === 'Enter') submitForm();");

// Media controls
video->setEventHandler("click", "togglePlayPause();");
video->setEventHandler("pointerdown", "startSeek(event);");
```

## Benefits

1. **Extensibility**: Support for any event type, not just onclick
2. **Event Objects**: Handlers receive proper event objects with useful properties
3. **Backward Compatibility**: Existing onclick code continues to work unchanged
4. **Standards Compliance**: Aligns with DOM event handler semantics
5. **Foundation for Growth**: Enables implementation of complete event handling system

## Testing

Comprehensive tests verify:
- Multiple event type support
- Backward compatibility with onclick methods
- Event object passing
- Handler replacement and coexistence
- Function reference support
- Error handling

## Migration Guide

### For New Code
Use the new generalized API:
```cpp
element->setEventHandler("pointerdown", "handlePointerDown(event);");
```

### For Existing Code
No changes needed - existing onclick methods continue to work:
```cpp
element->setOnClickHandler("handleClick();"); // Still works
```

### Gradual Migration
Existing onclick handlers can be gradually migrated:
```cpp
// Before
element->setOnClickHandler("handleClick();");

// After (equivalent)
element->setEventHandler("click", "handleClick(event);");
```

## Future Enhancements

This refactoring enables future improvements:
- Complete DOM event system implementation
- Event listener management (addEventListener/removeEventListener)
- Event delegation and bubbling
- Custom event types
- Performance optimizations

## Conclusion

The refactoring successfully generalizes event handling while maintaining complete backward compatibility. The new system provides a solid foundation for implementing a complete, standards-compliant event system in JSAR Runtime.