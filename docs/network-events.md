# Network Online/Offline Events Implementation

This implementation adds support for browser-standard network connectivity events to the JSAR runtime.

## Features

- ✅ `window.addEventListener('online', handler)` 
- ✅ `window.addEventListener('offline', handler)`
- ✅ `window.ononline = handler`
- ✅ `window.onoffline = handler`
- ✅ `navigator.onLine` property reflects current network status
- ✅ Compatible with Web API standards

## Usage

### Method 1: Event Listeners

```javascript
window.addEventListener('online', function() {
  console.log('Network is back online!');
  // Resume network operations
  syncData();
});

window.addEventListener('offline', function() {
  console.log('Network went offline!');
  // Switch to offline mode
  showOfflineMessage();
});
```

### Method 2: Event Handler Properties

```javascript
window.ononline = function() {
  console.log('Connected to network');
  document.getElementById('status').textContent = 'Online';
};

window.onoffline = function() {
  console.log('Disconnected from network');
  document.getElementById('status').textContent = 'Offline';
};
```

### Method 3: Check Current Status

```javascript
if (navigator.onLine) {
  // User is currently online
  fetchData();
} else {
  // User is currently offline
  loadCachedData();
}
```

## Implementation Details

### Files Modified

1. **`lib/events/network-events.ts`** - New file containing the network monitoring logic
2. **`lib/window.ts`** - Extended to make window an EventTarget with online/offline support
3. **`lib/navigator/index.ts`** - Updated to properly initialize navigator.onLine
4. **`lib/polyfills.ts`** - Integrated network monitoring initialization

### Architecture

```
NetworkStatusMonitor (monitors connectivity)
    ↓ dispatches events
WindowImpl (extends EventTarget)
    ↓ forwards to
window object (global)
    ↓ updates
navigator.onLine property
```

### Network Detection

The implementation uses a polling-based approach that checks network connectivity every 5 seconds. The detection logic:

1. Attempts a simple fetch request to a data URL
2. Falls back to conservative "online" assumption if detection fails
3. Dispatches events when status changes

### Compatibility

- Follows [MDN Web API standards](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/onLine)
- Compatible with existing browser code
- Works with both addEventListener and property-based event handlers
- Maintains all existing window object functionality

## Testing

Basic test coverage is provided in `lib/test/network-events.test.ts` which verifies:

- Window object has EventTarget capabilities
- Event listeners can be attached and removed
- Event handler properties work correctly
- navigator.onLine reflects network status
- Events are dispatched when status changes

## Example Integration

```javascript
// Complete example showing robust network handling
class NetworkAwareApp {
  constructor() {
    this.setupNetworkHandling();
    this.updateUIFromNetworkStatus();
  }
  
  setupNetworkHandling() {
    window.addEventListener('online', () => {
      this.onNetworkOnline();
    });
    
    window.addEventListener('offline', () => {
      this.onNetworkOffline();
    });
  }
  
  onNetworkOnline() {
    console.log('📶 Back online - syncing data...');
    this.syncPendingChanges();
    this.showNotification('Connected to network');
  }
  
  onNetworkOffline() {
    console.log('📵 Gone offline - switching to cached mode...');
    this.pauseNetworkOperations();
    this.showNotification('Working offline');
  }
  
  updateUIFromNetworkStatus() {
    const statusElement = document.getElementById('network-status');
    if (statusElement) {
      statusElement.textContent = navigator.onLine ? 'Online' : 'Offline';
      statusElement.className = navigator.onLine ? 'online' : 'offline';
    }
  }
  
  syncPendingChanges() {
    // Sync data that was queued while offline
  }
  
  pauseNetworkOperations() {
    // Stop any ongoing network requests
  }
  
  showNotification(message) {
    // Show user notification
  }
}

// Initialize the app
const app = new NetworkAwareApp();
```

This implementation enables JSAR applications to gracefully handle network connectivity changes, improving user experience and application robustness.