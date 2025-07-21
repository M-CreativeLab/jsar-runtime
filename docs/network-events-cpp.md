# Network Online/Offline Events Implementation in C++

This implementation provides network connectivity monitoring in JSAR using C++ and platform-specific APIs.

## Features Implemented

- ✅ **Platform-specific network detection**: Uses native APIs when available
  - Windows: Network Location Awareness (NLA) API
  - macOS/iOS: SystemConfiguration framework
  - Linux: Network interface checking
  - Android: Generic socket-based detection (placeholder for future JNI implementation)
- ✅ **Real-time monitoring**: Polls network status every 5 seconds
- ✅ **Navigator.onLine**: Property reflects current connectivity status
- ✅ **Window event handlers**: Support for `window.ononline` and `window.onoffline`
- ✅ **Thread-safe**: Uses atomic operations for thread safety

## Architecture

```
NetworkStatusMonitor (C++) → Navigator (C++) → Window (C++) → JavaScript bindings
     ↓ (platform APIs)           ↓ (.onLine)       ↓ (events)      ↓ (export)
Platform Network APIs      Navigator object   Window events    JS window/navigator
```

## Files Added/Modified

### New Files
- `src/client/dom/network_status_monitor.hpp` - Network monitoring interface
- `src/client/dom/network_status_monitor.cpp` - Platform-specific implementations
- `src/client/browser/navigator.hpp` - Navigator object with onLine property
- `src/client/browser/navigator.cpp` - Navigator implementation
- `src/client/bindings/network_bindings.hpp` - JavaScript binding interface
- `src/client/bindings/network_bindings.cpp` - Binding implementations

### Modified Files
- `src/client/browser/window.hpp` - Added ononline/onoffline properties and network event support
- `src/client/browser/window.cpp` - Implemented network event handling

## Usage (JavaScript)

```javascript
// Method 1: Event handler properties
window.ononline = function() {
  console.log('Network is back online!');
};

window.onoffline = function() {
  console.log('Network went offline!');
};

// Method 2: Check current status
if (navigator.onLine) {
  // User is currently online
  fetchData();
} else {
  // User is currently offline  
  loadCachedData();
}
```

## Platform-Specific Implementations

### Windows
Uses the Network Location Awareness (NLA) API via `INetworkListManager` to check for internet connectivity.

### macOS/iOS/tvOS
Uses the SystemConfiguration framework's `SCNetworkReachability` API to test reachability to a known server.

### Linux
Checks for active network interfaces using `getifaddrs()` to determine if any non-loopback interfaces are available.

### Android
Currently uses a generic socket-based approach. Can be enhanced with JNI calls to `ConnectivityManager`.

### Generic Fallback
Attempts a non-blocking socket connection to Google's DNS server (8.8.8.8:53) as a connectivity test.

## Integration Notes

- The network monitor starts automatically when the first Window is created
- Callbacks are used to notify windows of status changes
- The system is designed to be thread-safe and efficient
- Platform detection uses the existing `UNITY_*` macros from `platform_base.hpp`

## Future Enhancements

1. **DOM Events**: Add proper DOM event dispatching for `addEventListener('online'/'offline')`
2. **Android JNI**: Implement proper Android ConnectivityManager integration
3. **WebGL Support**: Add network status for WebGL builds
4. **Configuration**: Make polling interval configurable
5. **Error Handling**: Enhanced error handling and logging