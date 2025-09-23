# WebXR Event Bindings Migration

This document describes the migration of WebXR event bindings from N-API to V8 API using `scripting_base::ObjectWrap`.

## Overview

The WebXR event bindings have been successfully migrated from the N-API implementation in `src/bindings/dom/events/` to the V8-based implementation in `src/client/script_bindings/events/`.

## Migrated Event Classes

### XRSessionEvent (`xr_session_event`)
- **Location**: `src/client/script_bindings/events/xr_session_event.hpp/cpp`
- **Inherits from**: `Event` (via `scripting_base::ObjectWrap`)
- **Properties**:
  - `session`: The XRSession that fired the event
- **Usage**: Session lifecycle events (start, end, visibilitychange)

### XRInputSourceEvent (`xr_input_source_event`)
- **Location**: `src/client/script_bindings/events/xr_input_source_event.hpp/cpp`
- **Inherits from**: `Event` (via `scripting_base::ObjectWrap`)
- **Properties**:
  - `frame`: The XRFrame at the time of the event
  - `inputSource`: The XRInputSource that triggered the event
- **Usage**: Input events (select, selectstart, selectend, squeeze, etc.)

### XRInputSourcesChangeEvent (`xr_input_sources_change_event`)
- **Location**: `src/client/script_bindings/events/xr_input_sources_change_event.hpp/cpp`
- **Inherits from**: `Event` (via `scripting_base::ObjectWrap`)
- **Properties**:
  - `session`: The XRSession that fired the event
  - `added`: Array of newly connected XRInputSource objects
  - `removed`: Array of disconnected XRInputSource objects
- **Usage**: Input source connection/disconnection events

## Architecture

### Inheritance Hierarchy
```cpp
Event (base DOM event class)
├── XRSessionEvent
├── XRInputSourceEvent
└── XRInputSourcesChangeEvent
```

### V8 Integration Pattern
All WebXR events follow the established V8 binding pattern:

1. **Template-based inheritance**: Uses `scripting_base::ObjectWrap<T, D, B>`
2. **Property accessors**: V8 property getters using `v8::ObjectTemplate::SetAccessor`
3. **Factory methods**: Static `NewInstance` methods for object creation
4. **Configuration**: Static `ConfigureFunctionTemplate` for V8 setup

## JavaScript API Compatibility

The migrated events maintain full WebXR API compatibility:

```javascript
// XRSessionEvent usage
session.addEventListener('end', (event) => {
  console.log('Session ended:', event.session);
});

// XRInputSourceEvent usage
session.addEventListener('select', (event) => {
  console.log('Input source:', event.inputSource);
  console.log('Frame:', event.frame);
});

// XRInputSourcesChangeEvent usage
session.addEventListener('inputsourceschange', (event) => {
  console.log('Added input sources:', event.added);
  console.log('Removed input sources:', event.removed);
  console.log('Session:', event.session);
});
```

## Event Factory Integration

The event system includes factory methods in `all_events.hpp/cpp`:

- **`MakeEvent`**: Creates DOM events from native `dom::Event` objects
- **`MakeXREvent`**: Creates WebXR events from native XR event objects with type detection

## Benefits

1. **Zero N-API Dependencies**: Pure V8 API implementation
2. **Performance**: Direct V8 calls eliminate N-API marshalling overhead  
3. **Consistency**: Follows same patterns as DOM event bindings
4. **Type Safety**: Template-based inheritance ensures compile-time safety
5. **Standards Compliance**: Full WebXR event API compatibility

## Migration Status

✅ **COMPLETE**: All WebXR event classes from `src/bindings/dom/events/` migrated to V8 API
- [x] XRSessionEvent → `xr_session_event`
- [x] XRInputSourceEvent → `xr_input_source_event`
- [x] XRInputSourcesChangeEvent → `xr_input_sources_change_event`

The migration demonstrates consistent naming conventions following the requested `xr_input_source_event` pattern and complete API compatibility.