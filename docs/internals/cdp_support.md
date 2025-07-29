# Chrome DevTools Protocol (CDP) Support

This document describes the CDP support added to the JSAR Runtime inspector.

## Overview

The JSAR Runtime inspector now supports basic Chrome DevTools Protocol (CDP) functionality, enabling CDP clients (such as Chrome DevTools) to connect via WebSocket and interact with the runtime.

## Architecture

### Core Components

- **`CdpHandler`** - Main coordinator for CDP message processing
- **`CdpMessage`** - CDP message parsing with JSON validation
- **`CdpResponse`** - Response builder for success/error/event responses
- **`CdpDomainHandler`** - Base class for domain implementations

### Domain Implementations

- **`CdpRuntimeDomain`** - Runtime domain with JavaScript runtime methods
- **`CdpMyExampleDomain`** - Example domain for testing connectivity
- **`CdpJsarUniversalRenderingServerDomain`** - Universal Rendering Server domain for debugging and controlling the unified rendering backend

## Supported Domains

### Runtime Domain

The Runtime domain exposes JavaScript runtime functionality.

#### Methods

- **`Runtime.enable`** - Enables runtime domain
- **`Runtime.disable`** - Disables runtime domain  
- **`Runtime.getVersion`** - Returns JavaScript runtime version information

#### Example Usage

```javascript
// Enable runtime domain
{
  "id": 1,
  "method": "Runtime.enable",
  "params": {}
}

// Response
{
  "id": 1,
  "result": {}
}
```

```javascript
// Get version information
{
  "id": 2,
  "method": "Runtime.getVersion",
  "params": {}
}

// Response
{
  "id": 2,
  "result": {
    "product": "JSAR",
    "revision": "0.8.2",
    "userAgent": "JSAR/0.8.2",
    "jsVersion": "ES2021"
  }
}
```

### MyExample Domain

Sample domain for testing CDP connectivity and method invocation.

#### Methods

- **`MyExample.ping`** - Simple ping command that responds with pong
- **`MyExample.echo`** - Echoes back the provided parameters
- **`MyExample.getInfo`** - Returns information about this domain

#### Example Usage

```javascript
// Ping test
{
  "id": 3,
  "method": "MyExample.ping",
  "params": {}
}

// Response
{
  "id": 3,
  "result": {
    "message": "pong",
    "timestamp": 1234567890
  }
}
```

```javascript
// Echo test
{
  "id": 4,
  "method": "MyExample.echo",
  "params": {
    "message": "Hello JSAR!",
    "data": { "test": true }
  }
}

// Response
{
  "id": 4,
  "result": {
    "echoed": {
      "message": "Hello JSAR!",
      "data": { "test": true }
    }
  }
}
```

## Protocol Discovery

The inspector provides protocol discovery via the `/json/protocol` endpoint:

```bash
curl http://localhost:9423/json/protocol
```

This returns the complete protocol description including all supported domains and their methods.

## Connection

### WebSocket Endpoint

```
ws://localhost:9423/devtools/inspector/{content_id}
```

### Example Connection

```javascript
const ws = new WebSocket('ws://localhost:9423/devtools/inspector/1');

ws.onopen = () => {
  // Send CDP message
  ws.send(JSON.stringify({
    id: 1,
    method: 'Runtime.enable',
    params: {}
  }));
};

ws.onmessage = (event) => {
  const response = JSON.parse(event.data);
  console.log('CDP Response:', response);
};
```

## Error Handling

The implementation follows JSON-RPC 2.0 error codes:

- **-32700** - Parse error (invalid JSON)
- **-32601** - Method not found (unknown domain or method)
- **-32603** - Internal error (exception during processing)

### Example Error Response

```javascript
{
  "id": 5,
  "error": {
    "code": -32601,
    "message": "Method not found"
  }
}
```

## Extensibility

### Adding New Domains

1. Create a new domain handler class extending `CdpDomainHandler`
2. Implement the `handleMethod` function
3. Register the domain in `TrInspector::initialize()`

#### Example

```cpp
class CdpCustomDomain : public CdpDomainHandler {
public:
    std::string handleMethod(const std::string& method, const CdpMessage& message) override {
        if (method == "customMethod") {
            rapidjson::Document result;
            result.SetObject();
            // ... build result
            return CdpResponse::success(message.id, result);
        }
        return CdpResponse::error(message.id, -32601, "Custom." + method + " is not supported");
    }
};

// Register in TrInspector::initialize()
cdpHandler_->registerDomain("Custom", std::make_unique<CdpCustomDomain>());
```

## Logging

The implementation includes comprehensive logging for debugging:

- Connection events
- Message parsing
- Method invocation
- Error handling

Logs use the `LOG_TAG_INSPECTOR` tag and can be viewed in the inspector logs.

## Testing

Use the provided test clients:

- **Node.js Client**: `fixtures/inspector-client/inspector_websocket_client.js`
- **Browser Test**: `fixtures/inspector-client/inspector_websocket_test.html`

Or connect with Chrome DevTools using the DevTools frontend URL from `/json/list`.

## Compatibility

The implementation is compatible with:

- Chrome DevTools Protocol v1.3
- Standard CDP clients
- WebSocket-based debugging tools
- JSON-RPC 2.0 message format

## JSAR.UniversalRenderingServer Domain

The JSAR.UniversalRenderingServer domain provides debugging and control for the unified rendering backend located in `src/renderer`.

### Methods

- **`JSAR.UniversalRenderingServer.enableTracing`** - Enable tracing in TrRenderer
- **`JSAR.UniversalRenderingServer.disableTracing`** - Disable tracing in TrRenderer
- **`JSAR.UniversalRenderingServer.setClientFrameRate`** - Control the client-side FPS (requires frameRate parameter)
- **`JSAR.UniversalRenderingServer.getRendererInfo`** - Get current renderer state information
- **`JSAR.UniversalRenderingServer.getContentRenderers`** - Get list of all content renderer instances for debugging
- **`JSAR.UniversalRenderingServer.getCommandBuffers`** - Get command buffer debugging information and inspection details

### Example Usage

```javascript
// Enable tracing
{
  "id": 10,
  "method": "JSAR.UniversalRenderingServer.enableTracing",
  "params": {}
}

// Response
{
  "id": 10,
  "result": {
    "success": true,
    "tracingEnabled": true
  }
}

// Set client frame rate
{
  "id": 11,
  "method": "JSAR.UniversalRenderingServer.setClientFrameRate",
  "params": {
    "frameRate": 75
  }
}

// Response
{
  "id": 11,
  "result": {
    "success": true,
    "frameRate": 75
  }
}

// Get renderer information
{
  "id": 12,
  "method": "JSAR.UniversalRenderingServer.getRendererInfo",
  "params": {}
}

// Response
{
  "id": 12,
  "result": {
    "tracingEnabled": true,
    "fps": 60,
    "clientFrameRate": 75,
    "uptime": 123,
    "stencilClearDisabled": false,
    "hostContextSummaryEnabled": false,
    "appContextSummaryEnabled": false,
    "useDoubleWideFramebuffer": false,
    "commandBufferPort": 9424
  }
}
```
