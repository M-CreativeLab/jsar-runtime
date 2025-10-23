# Notification API Implementation

This directory contains the implementation of the Web Notification API for JSAR Runtime.

## Files

### C++ Core Implementation
- `src/client/dom/notification.hpp` - Notification class header with permission enums and options
- `src/client/dom/notification.cpp` - Notification class implementation

### JavaScript Bindings
- `src/client/script_bindings/notification.hpp` - V8 wrapper class header
- `src/client/script_bindings/notification.cpp` - V8 wrapper implementation

### Tests
- `tests/client/notification_tests.cpp` - C++ unit tests using Catch2
- `tests/fixtures/notification-example.js` - JavaScript usage example

## API Overview

### Constructor
```javascript
const notification = new Notification(title, options);
```

### Static Properties
- `Notification.permission` - Returns the current permission state ("default", "granted", or "denied")

### Static Methods
- `Notification.requestPermission()` - Requests permission to show notifications (stub implementation)

### Instance Properties (Read-only)
- `title` - The title of the notification
- `body` - The body text of the notification
- `dir` - Text direction ("auto", "ltr", or "rtl")
- `lang` - Language code
- `tag` - Notification tag for grouping
- `icon` - Icon URL
- `badge` - Badge URL
- `sound` - Sound URL
- `renotify` - Whether to notify again if replacing an existing notification
- `requireInteraction` - Whether the notification requires user interaction to dismiss
- `silent` - Whether the notification should be silent
- `data` - Custom data (currently returns null)

### Event Handlers
- `onshow` - Fired when the notification is displayed
- `onclick` - Fired when the notification is clicked
- `onclose` - Fired when the notification is closed
- `onerror` - Fired when an error occurs

### Instance Methods
- `close()` - Closes the notification programmatically

## Current Status

### Implemented ✅
- Full C++ Notification class with all standard properties
- JavaScript bindings for constructor and all properties/methods
- Static permission API (stub implementation)
- Event handler properties (storage, not yet dispatching)
- Unit tests for C++ implementation

### TODO 🚧
- Platform-specific notification display:
  - macOS: NSUserNotification API
  - Windows: Toast Notification API
  - Linux: libnotify
  - XR: JSAR internal 3D UI
- Event dispatching (show, click, close, error events)
- Promise-based `requestPermission()` (currently synchronous stub)
- Data property support (currently returns null)
- Persistent permission storage
- Notification action buttons
- Vibration patterns

## Usage Example

See `tests/fixtures/notification-example.js` for a complete JavaScript example.

Basic usage:
```javascript
// Check permission
console.log(Notification.permission); // "default"

// Request permission (stub - auto-grants)
Notification.requestPermission();

// Create notification
const notification = new Notification('Hello!', {
  body: 'This is a test notification',
  icon: '/icon.png'
});

// Handle events
notification.onclick = () => {
  console.log('Clicked!');
  notification.close();
};
```

## References

- [W3C Notification API Specification](https://www.w3.org/TR/notifications/)
- [MDN Notification Documentation](https://developer.mozilla.org/en-US/docs/Web/API/Notification)
- [Chromium Implementation Reference](https://source.chromium.org/chromium/chromium/src/+/main:third_party/blink/renderer/modules/notifications/notification.cc)
