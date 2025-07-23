# JSAR Extension System (C++)

The JSAR Extension System provides Chrome Extension-like functionality for the JSAR runtime, implementing a browser-scoped extension architecture with proper process separation between background scripts and content scripts.

## Overview

The extension system is implemented in C++ following Chrome Extension architecture and provides:

- Extension loading and unloading with process separation
- Background script execution in dedicated processes (forked)
- Content script injection into page renderer processes
- Extension lifecycle management with proper state handling
- Event-driven architecture for extension communication
- Manifest-based configuration supporting content_scripts

## Architecture

### Core Components

1. **Extension** (`src/runtime/extension/extension.hpp/.cpp`)
   - Represents an individual extension
   - Manages extension lifecycle (loading, enabling, disabling, unloading)
   - Forks background processes for background script execution
   - Handles content script injection based on URL patterns
   - Provides extension API context with Chrome APIs

2. **ExtensionManager** (`src/runtime/extension/extension_manager.hpp/.cpp`)
   - Browser-scoped manager for all extensions
   - Handles extension loading from directories
   - Coordinates content script injection across all extensions
   - Manages background process lifecycle
   - Provides batch operations and statistics

3. **Types** (`src/runtime/extension/extension_types.hpp`)
   - Extension manifest interface compatible with Chrome Extension v3
   - Extension state enumeration and process information
   - Content script configuration and injection context
   - Event and API type definitions

### Process Architecture

Following Chrome Extension patterns:

| Component           | Execution Context               | Capabilities                                | Lifecycle                  |
|---------------------|----------------------------------|---------------------------------------------|----------------------------|
| **Background Script** | Extension Process (forked)     | Full Chrome API access                      | Persistent process per extension |
| **Content Script**   | Page's Renderer Process         | DOM access + limited Chrome APIs            | Tied to page lifecycle     |

### Integration

The extension system integrates with the JSAR runtime at the browser level in `src/runtime/extension/`:

- Extension manager manages all extension processes
- Background scripts fork dedicated processes with full API access
- Content scripts inject into page renderer processes on URL matches
- Event handling for extension lifecycle and communication

## Extension Structure

Extensions follow Chrome Extension v3 directory structure:

```
my-extension/
├── manifest.json         # Extension metadata and configuration
├── background.js         # Background script (executed in dedicated process)
└── content.js           # Content script (injected into pages)
```

### Manifest Format

**manifest.json** supports Chrome Extension v3 features:
```json
{
  "name": "My Extension",
  "version": "1.0.0", 
  "manifest_version": 3,
  "description": "Extension description",
  "background": {
    "scripts": ["background.js"],
    "persistent": false
  },
  "content_scripts": [{
    "matches": ["https://*.example.com/*", "http://localhost/*"],
    "js": ["content.js"],
    "run_at": "document_idle",
    "all_frames": false
  }],
  "permissions": ["tabs", "storage"]
}
```

### Content Script Injection

#### 1. Declarative Injection (via manifest.json)
```json
"content_scripts": [{
  "matches": ["https://*.example.com/*"],
  "js": ["content.js"],
  "run_at": "document_idle",  // Options: document_start/end/idle
  "all_frames": false         // Inject into subframes?
}]
```

Trigger Conditions:
- URL matches pattern using regex-based matching
- Initial load or full page navigation
- Executed at specified timing (document_start/end/idle)

#### 2. Programmatic Injection (Future)

```cpp
// Future API for dynamic injection
chrome.scripting.executeScript({
  target: {tabId: tab.id},
  files: ['dynamic.js'],
  injectImmediately: true
});
```

### Background Script Execution

Background scripts execute in dedicated forked processes:

**background.js:**
```javascript
// Extension loaded in dedicated process - full Chrome API access available
console.log('Extension loaded in background process!');
console.log('Process ID:', process.pid);

// Environment variables available:
console.log('Extension ID:', process.env.EXTENSION_ID);
console.log('Extension name:', process.env.EXTENSION_NAME);
console.log('Extension version:', process.env.EXTENSION_VERSION);

// TODO: Full Chrome APIs will be available
// if (chrome && chrome.extension) {
//   console.log('Extension ID:', chrome.extension.id);
// }
## Usage Example

```cpp
#include "src/runtime/extension/extension_manager.hpp"

// Initialize extension manager (browser-scoped)
auto extension_manager = std::make_unique<jsar::extensions::ExtensionManager>();

// Load single extension
bool loaded = extension_manager->loadExtension("/path/to/extension");

// Load all extensions from directory
bool batch_loaded = extension_manager->loadExtensionsFromDirectory("/path/to/extensions");

// Inject content scripts for a URL (called when page loads)
bool injected = extension_manager->injectContentScriptsForUrl("https://example.com/page");

// Extension lifecycle management
auto extension = extension_manager->getExtension("extension-id");
if (extension) {
  extension->enable();   // Start background processes
  extension->disable();  // Stop background processes
}

// Statistics
size_t active_processes = extension_manager->getActiveBackgroundProcessCount();
size_t total_extensions = extension_manager->getExtensionCount();
```

## Security & Performance

- **Process Isolation**: Background scripts run in separate forked processes
- **Isolated World**: Content scripts run in separate JS context (to be implemented)
- **Limited API Access**: Content scripts have restricted Chrome API access
- **URL Pattern Matching**: Content scripts only inject on matching URLs
- **Process Management**: Automatic cleanup of background processes on extension unload

## Implementation Status

### Completed ✅
- Extension manifest parsing with content_scripts support
- Background script process forking with environment setup
- Content script URL pattern matching and injection framework
- Extension lifecycle management with proper process cleanup
- Browser-scoped extension manager architecture

### In Progress 🚧
- JavaScript execution context for background processes
- Content script injection into actual page renderer processes
- Chrome Extension API implementation (chrome.tabs, chrome.storage, etc.)
- Message passing between background and content scripts

### Future 🔮
- Popup and options pages
- Advanced permission systems
- Extension store and auto-updates
- Developer tools integration

## Implementation Summary

The extension system now follows Chrome Extension architecture with proper process separation:

1. **Extension implementation moved to `src/runtime/extension`** (browser-scoped concept)
2. **Background scripts fork dedicated processes** for execution with full API access
3. **Content scripts inject into page renderer processes** based on URL matching
4. **Proper security isolation** between extension and content contexts
5. **Complete lifecycle management** with process cleanup and state tracking

This provides a solid foundation for Chrome Extension compatibility while maintaining security and performance through process separation.