# JSAR Extension System (C++)

The JSAR Extension System provides Chrome Extension-like functionality for the JSAR runtime, allowing developers to create and load extensions that can execute scripts and extend the runtime's capabilities.

## Overview

The extension system is implemented in C++ and inspired by Chrome Extension architecture and provides:

- Extension loading and unloading
- Background script execution
- Extension lifecycle management
- Event-driven architecture
- Manifest-based configuration

## Architecture

### Core Components

1. **Extension** (`src/extensions/extension.hpp/.cpp`)
   - Represents an individual extension
   - Manages extension lifecycle (loading, enabling, disabling, unloading)
   - Executes background scripts (stub implementation)
   - Provides extension API context

2. **ExtensionManager** (`src/extensions/extension_manager.hpp/.cpp`)
   - Manages multiple extensions
   - Handles extension loading from directories
   - Provides batch operations
   - Emits extension lifecycle events

3. **Types** (`lib/extensions/types.ts`)
   - Extension manifest interface
   - Extension state enumeration
   - Event and API type definitions

### Integration

The extension system integrates with the main JSAR runtime through the `TransmuteRuntime2` class, which includes:

- Extension manager instance
- Extension loading methods
- Event handling for extension lifecycle
- Cleanup on runtime shutdown

## Extension Structure

Extensions follow a directory-based structure similar to Chrome Extensions:

```
my-extension/
├── manifest.json          # Extension metadata and configuration
├── background.js          # Background script (optional)
└── README.md             # Documentation (optional)
```

### Manifest Format

The `manifest.json` file defines extension metadata:

```json
{
  "name": "My Extension",
  "version": "1.0.0",
  "description": "Description of my extension",
  "manifest_version": 3,
  "background": {
    "scripts": ["background.js"],
    "persistent": false
  },
  "permissions": ["storage"]
}
```

### Required Fields

- `name`: Extension name
- `version`: Extension version (semver format)
- `manifest_version`: Manifest format version (use 3)

### Optional Fields

- `description`: Human-readable description
- `background`: Background script configuration
  - `scripts`: Array of background script files
  - `persistent`: Whether the background script should persist
- `permissions`: Array of permissions (for future use)

## Usage

### Loading Extensions

#### Load Single Extension

```typescript
import { TransmuteRuntime2 } from './runtime2';

const runtime = new TransmuteRuntime2(gl, runtimeId);

// Load a single extension (currently uses C++ stub)
await runtime.loadExtension('/path/to/extension');
```

#### Load Multiple Extensions

```typescript
// Load all extensions from a directory (currently uses C++ stub)
await runtime.loadExtensionsFromDirectory('/path/to/extensions');
```

### Extension Manager API

```typescript
// Currently using C++ stub implementation
// Direct C++ ExtensionManager access will be available via bindings

const runtime = new TransmuteRuntime2(gl, runtimeId);
const extensionManager = runtime.extensionManager;

// Load extension (C++ implementation)
await extensionManager.loadExtension('/path/to/extension');

// Load multiple extensions (C++ implementation)
await extensionManager.loadExtensionsFromDirectory('/path/to/extensions');

// Unload all extensions (C++ implementation)
await extensionManager.unloadAllExtensions();
```

### Extension Lifecycle (C++ Implementation)

Extensions go through the following states:

1. **LOADING** - Extension is being loaded and manifest parsed
2. **LOADED** - Extension is loaded but not yet enabled
3. **RUNNING** - Extension is active and background scripts executed
4. **DISABLED** - Extension is loaded but temporarily disabled
5. **ERROR** - Extension encountered an error
6. **UNLOADED** - Extension has been completely unloaded

### Background Scripts

Background scripts are executed when an extension is loaded. They have access to a basic extension API:

```javascript
// background.js
console.log('Extension loaded!');

// Access extension information
if (chrome && chrome.extension) {
  console.log('Extension ID:', chrome.extension.id);
  console.log('Extension manifest:', chrome.extension.manifest);
}

// Access runtime APIs
if (chrome && chrome.runtime) {
  chrome.runtime.onStartup.addListener(() => {
    console.log('Extension startup event triggered');
  });
  
  const iconUrl = chrome.runtime.getURL('icon.png');
  console.log('Extension icon URL:', iconUrl);
}
```

### Event Handling

Listen for extension lifecycle events:

```typescript
extensionManager.onExtensionEvent('extensionLoaded', (event) => {
  console.log('Extension loaded:', event.extensionId);
});

extensionManager.onExtensionEvent('extensionError', (event) => {
  console.error('Extension error:', event.extensionId, event.data);
});
```

## Extension API

Extensions have access to a limited Chrome Extension-compatible API:

### chrome.extension

- `id` - Extension identifier
- `manifest` - Extension manifest object
- `state` - Current extension state

### chrome.runtime

- `getURL(path)` - Get URL for extension resource
- `onStartup` - Startup event listener
- `onInstalled` - Installation event listener

## Examples

### Sample Extension

See `examples/extensions/sample-extension/` for a complete example extension that demonstrates:

- Basic manifest.json structure
- Background script execution
- Extension API usage
- Console logging

### Testing

Test the extension system:

```bash
# Run the test script
node tests/test-extension-system.js
```

## Future Enhancements

The extension system is designed to be extensible. Future enhancements may include:

- Content scripts for page injection
- Message passing between extensions and runtime
- Storage APIs for extension data
- Popup and options pages
- More Chrome Extension APIs
- Extension packaging and distribution
- Security sandboxing
- Permission system implementation

## Security Considerations

Currently, the extension system provides basic script execution without sandboxing. In a production environment, consider:

- Script sandboxing and isolation
- Permission validation
- Resource access control
- Extension signature verification
- Secure extension storage

## Troubleshooting

### Common Issues

1. **Extension fails to load**
   - Check manifest.json syntax
   - Verify required fields are present
   - Check file permissions

2. **Background script errors**
   - Check JavaScript syntax
   - Verify API usage
   - Check console output for error details

3. **Extension state issues**
   - Ensure proper lifecycle management
   - Check for unhandled promise rejections
   - Verify extension cleanup

### Debug Output

The extension system provides detailed console logging:

```
[ExtensionManager] Loading extension from: /path/to/extension
[Extension:extension-id] Executed background script: background.js
[Runtime:123] Extension loaded: extension-id
```

Enable debug logging by checking the console output when loading and managing extensions.