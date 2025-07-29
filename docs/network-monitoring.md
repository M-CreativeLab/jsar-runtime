# Network Status Monitoring

This implementation adds support for monitoring network connectivity status in JSAR runtime, providing web-standard compatible APIs for detecting online/offline states.

## Features

- **Cross-platform support**: Android (ConnectivityManager) and macOS (SCNetworkReachability)
- **Web-standard API**: Compatible with standard browser online/offline events
- **Event-driven**: Real-time network status change notifications
- **Navigator API**: Standard `navigator.onLine` property support

## Usage

### JavaScript API

```javascript
// Listen for network status changes
window.addEventListener('online', function() {
  console.log('Network is available');
});

window.addEventListener('offline', function() {
  console.log('Network is unavailable');
});

// Check current status
if (navigator.onLine) {
  console.log('Currently online');
} else {
  console.log('Currently offline');
}
```

### Event Handling Example

```javascript
// Queue requests when offline
let offlineQueue = [];

window.addEventListener('offline', function() {
  console.log('Going offline - will queue requests');
});

window.addEventListener('online', function() {
  console.log('Back online - processing queued requests');
  offlineQueue.forEach(processRequest);
  offlineQueue = [];
});

function makeRequest(data) {
  if (navigator.onLine) {
    processRequest(data);
  } else {
    offlineQueue.push(data);
  }
}
```

## Implementation Details

### Architecture

1. **NetworkMonitor**: Platform-specific network monitoring
   - `AndroidNetworkMonitor`: Uses ConnectivityManager APIs
   - `MacOSNetworkMonitor`: Uses SCNetworkReachabilitySetCallback
   - Fallback implementation for other platforms

2. **NetworkService**: Manages monitoring and event dispatch
   - Singleton service for global network state
   - Integrates with JSAR event system
   - Dispatches online/offline events

3. **DOM Integration**: Standard web API support
   - `NetworkEvent`: DOM events for network changes
   - Window event target integration
   - Navigator object with onLine property

### Platform Support

- **macOS**: Full implementation using System Configuration framework
- **Android**: Foundation ready for ConnectivityManager integration
- **Other platforms**: Basic fallback (assumes online)

### Files Added

#### Runtime Core
- `src/runtime/network_monitor.hpp/cpp` - Platform-specific monitoring
- `src/runtime/network_service.hpp/cpp` - Service management
- `src/runtime/network_events.hpp` - Event type definitions

#### DOM Integration  
- `src/client/dom/events/network_event.hpp/cpp` - DOM event classes
- `src/bindings/dom/events/network_event.hpp/cpp` - JavaScript bindings

#### Browser Integration
- `src/bindings/browser/navigator.hpp/cpp` - Navigator object
- Updates to `src/client/browser/window.hpp/cpp` - Window integration
- Updates to `src/bindings/browser/window.hpp/cpp` - JavaScript bindings

#### Tests and Examples
- `tests/runtime/network_tests.cpp` - Unit tests
- `tests/runtime/network_integration_tests.cpp` - Integration tests  
- `examples/network_monitoring_example.js` - Usage example

## Standards Compliance

The implementation follows web standards for network status monitoring:

- [MDN: Navigator.onLine](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/onLine)
- [MDN: Online/Offline Events](https://developer.mozilla.org/en-US/docs/Web/API/Navigator/Online_and_offline_events)
- [HTML Living Standard](https://html.spec.whatwg.org/multipage/workers.html#navigator.online)

## Future Enhancements

1. Complete Android ConnectivityManager implementation
2. Add NetworkInformation API support for connection type/speed
3. Add offline storage capabilities
4. Implement retry mechanisms for failed requests

## Related

- Issue #113: Support online/offline network status monitoring
- Issue #160: Implement network status monitoring in src/runtime (Android/macOS)