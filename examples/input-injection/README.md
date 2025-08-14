# Keyboard Input Injection Demo

This example demonstrates how host applications can inject keyboard events into JSAR Runtime using the InputManager module.

## Overview

The InputManager module provides a clean separation between event capture and event management, allowing different platforms (Unity, Unreal, native, etc.) to implement their own input event listeners and inject events into JSAR.

## Architecture

```
Host Application (Unity/Unreal/Native)
    ↓ (captures input)
InputManager
    ↓ (IPC via InputEvent)
JSAR Client Process
    ↓ (DOM events)
Web Application
```

## Files

- `keyboard_demo.cpp` - C++ demo showing how to inject keyboard events
- `keyboard_demo.html` - HTML page that receives and displays keyboard events
- `README.md` - This file

## Usage

### 1. Host-Side Event Injection

```cpp
#include "runtime/embedder.hpp"
#include "runtime/input-manager/input_manager.hpp"

class MyApp : public TrEmbedder {
public:
    MyApp() : TrEmbedder(TrHostEngine::Unity) {}
    
    void onKeyPress(const std::string& key, const std::string& code) {
        // Create keyboard event data
        auto eventData = input_manager::keyboard_events::createKeyDownEvent(
            key, code, false, false, false, false);
        
        // Inject to all active content runtimes
        broadcastKeyboardEvent(eventData);
        
        // Or inject to specific content
        // injectKeyboardEvent(contentId, eventData);
    }
    
    bool onEvent(events_comm::TrNativeEvent &event, 
                 std::shared_ptr<TrContentRuntime> content) override {
        return true;
    }
};
```

### 2. Client-Side Event Handling

The injected events are automatically converted to standard DOM KeyboardEvents and dispatched to the appropriate elements:

```javascript
// Standard DOM event listeners work automatically
document.addEventListener('keydown', function(event) {
    console.log('Key pressed:', event.key, event.code);
    console.log('Modifiers:', {
        alt: event.altKey,
        ctrl: event.ctrlKey,
        meta: event.metaKey,
        shift: event.shiftKey
    });
});

// Events are dispatched to focused elements
inputElement.addEventListener('keydown', function(event) {
    // Handle input-specific keyboard events
});
```

## API Reference

### KeyboardEvent Helper Functions

```cpp
// Create key down event
auto keyDown = input_manager::keyboard_events::createKeyDownEvent(
    "a",           // key value
    "KeyA",        // physical key code
    false,         // altKey
    true,          // ctrlKey
    false,         // metaKey
    false          // shiftKey
);

// Create key up event
auto keyUp = input_manager::keyboard_events::createKeyUpEvent(
    "Enter", "Enter", false, false, false, false);

// Create key press event
auto keyPress = input_manager::keyboard_events::createKeyPressEvent(
    "Space", "Space", false, false, false, false);
```

### InputManager Methods

```cpp
// Inject event to specific content runtime
bool injectKeyboardEvent(uint32_t contentId, const TrKeyboardEventData& eventData);

// Broadcast event to all active content runtimes
int broadcastKeyboardEvent(const TrKeyboardEventData& eventData);

// Get list of active content runtime IDs
std::vector<uint32_t> getActiveContentIds() const;
```

## Key Mapping Examples

| Key | Value | Code | Description |
|-----|-------|------|-------------|
| a-z | "a" | "KeyA" | Letter keys |
| 0-9 | "0" | "Digit0" | Number keys |
| Space | "Space" | "Space" | Space bar |
| Enter | "Enter" | "Enter" | Enter key |
| Escape | "Escape" | "Escape" | Escape key |
| Control | "Control" | "ControlLeft" | Ctrl key |
| Alt | "Alt" | "AltLeft" | Alt key |
| Shift | "Shift" | "ShiftLeft" | Shift key |

## Event Flow

1. **Host Capture**: Host application (Unity, Unreal, etc.) captures input events using platform-specific APIs
2. **Event Creation**: Host creates `TrKeyboardEventData` using helper functions
3. **Injection**: Host calls `injectKeyboardEvent()` or `broadcastKeyboardEvent()`
4. **IPC Transport**: InputManager sends `InputEvent` via IPC to client process
5. **DOM Conversion**: Client converts to standard `KeyboardEvent`
6. **Dispatch**: Event is dispatched to focused element or document body
7. **Application Handling**: Web application handles event using standard DOM APIs

## Integration Guide

### Unity Integration Example

```csharp
// Unity C# script
using UnityEngine;

public class JSARInputForwarder : MonoBehaviour {
    private JSARRuntime jsar;
    
    void Start() {
        jsar = GetComponent<JSARRuntime>();
    }
    
    void Update() {
        foreach (KeyCode key in System.Enum.GetValues(typeof(KeyCode))) {
            if (Input.GetKeyDown(key)) {
                string keyValue = KeyCodeToString(key);
                string keyCode = KeyCodeToCode(key);
                jsar.InjectKeyDown(keyValue, keyCode);
            }
        }
    }
}
```

### Native Application Example

```cpp
// Platform-specific input capture
void OnPlatformKeyEvent(int virtualKey, bool isDown) {
    std::string key = VirtualKeyToString(virtualKey);
    std::string code = VirtualKeyToCode(virtualKey);
    
    if (isDown) {
        auto eventData = input_manager::keyboard_events::createKeyDownEvent(
            key, code, GetModifierState());
        embedder->broadcastKeyboardEvent(eventData);
    }
}
```

## Benefits

- **Platform Independence**: Host applications retain control over input capture
- **Standard Compliance**: Generates standard DOM KeyboardEvents
- **Performance**: Efficient IPC mechanism for event forwarding
- **Flexibility**: Supports both targeted and broadcast event injection
- **Accessibility**: Enables keyboard accessibility features in spatial applications

## Testing

Run the C++ demo to see keyboard event injection in action:

```bash
# Compile and run the demo (when build system supports it)
make examples
./build/examples/keyboard_demo
```

Open the HTML demo in JSAR Runtime to see events being received and handled.