# Chrome DevTools Protocol (CDP) Support

This document describes the CDP support added to the JSAR Runtime inspector.

## Overview

The JSAR Runtime inspector now supports basic Chrome DevTools Protocol (CDP) functionality, enabling CDP clients (such as Chrome DevTools) to connect via WebSocket and interact with the runtime. The system supports both host-side domain implementations and content process domain implementations through an IPC-based proxy system.

## Architecture

### Core Components

- **`CdpHandler`** - Main coordinator for CDP message processing in the host process
- **`CdpMessage`** - CDP message parsing with JSON validation
- **`CdpResponse`** - Response builder for success/error/event responses
- **`CdpDomainHandler`** - Base class for host-side domain implementations
- **`ContentDomainProxy`** - Proxy that forwards CDP requests to content processes via IPC
- **`ContentCdpHandler`** - Content-side coordinator for CDP message processing
- **`ContentCdpDomainHandler`** - Base class for content-side domain implementations

### Domain Implementations

#### Host-Side Domains
- **`CdpJsarUniversalRenderingServerDomain`** - Universal Rendering Server domain for debugging and controlling the unified rendering backend

#### Content-Side Domains (via Proxy)
- **`ContentCdpRuntimeDomain`** - Runtime domain with JavaScript runtime methods (runs in content processes)
- **`ContentCdpExampleDomain`** - Example domain for testing connectivity (runs in content processes)
- **`ContentCdpLogDomain`** - Log domain for collecting and broadcasting log entries from content processes
- **`ContentCdpDomDomain`** - DOM domain for accessing and manipulating the Document Object Model (runs in content processes)

### IPC Communication

The system uses a dedicated `inspector` IPC channel for communication between host and content processes:

- **`TrInspectorCommandMessage`** - IPC message format for CDP data
- **`TrCdpRequest`** - IPC message for forwarding CDP requests to content processes
- **`TrCdpResponse`** - IPC message for returning CDP responses from content processes  
- **`TrCdpEvent`** - IPC message for sending CDP events from content processes to host and then to CDP clients

### Event Broadcasting System

The implementation includes a real-time event broadcasting system:

- **`CdpEventSender`** - Callback mechanism in content processes for sending events
- **Content to Host Events** - Content processes send `TrCdpEvent` IPC messages to the host
- **Host Event Broadcasting** - `TrInspector::broadcastEventToClients()` forwards events to all connected WebSocket clients
- **Real-time DevTools Integration** - Events appear immediately in Chrome DevTools (e.g., log entries in Console)

### Request Flow

1. CDP client sends request to host process via WebSocket
2. `CdpHandler` receives and parses the request
3. If domain should be forwarded to content process:
   - `ContentDomainProxy` forwards request via `inspector` IPC channel
   - Content process `ContentCdpHandler` processes request
   - Response sent back via IPC to host process
   - Host process returns response to CDP client
4. If domain is host-side:
   - Request processed directly by host-side domain handler
   - Response returned immediately to CDP client

### Event Flow

1. Content process generates event (e.g., log entry added)
2. Content domain handler calls event sender callback
3. `TrCdpEvent` IPC message sent to host process
4. Host `TrInspector::broadcastEventToClients()` forwards to all WebSocket clients
5. Event appears in real-time in connected DevTools instances

### Memory Management & Architecture

- **RAII Compliance**: All inspector IPC channels use `unique_ptr` for automatic memory management
- **Instance-based Communication**: Each `TrContentRuntime` has associated `CdpHandler` reference for response routing
- **Singleton Logger**: Global `logging::Logger::GetInstance()` for client-side logging with automatic CDP integration
- **Conditional Compilation**: All inspector features guarded by `TR_ENABLE_INSPECTOR` preprocessor directives

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
    "revision": "0.9.0",
    "userAgent": "JSAR/0.9.0",
    "jsVersion": "ES2021"
  }
}
```

### Log Domain

The Log domain provides Chrome DevTools Protocol Log functionality for collecting and viewing log entries from content processes.

#### Methods

- **`Log.enable`** - Enables log domain and sends buffered log entries to DevTools
- **`Log.disable`** - Disables log domain and stops log event broadcasting
- **`Log.clear`** - Clears the log entry buffer in the content process
- **`Log.startViolationsReport`** - Starts performance violations reporting with configurable violation types
- **`Log.stopViolationsReport`** - Stops performance violations reporting

#### Events

- **`Log.entryAdded`** - Sent when a new log entry is added (real-time broadcasting to DevTools)

#### Log Entry Structure

Log entries follow the CDP Log.LogEntry specification:

```javascript
{
  "source": "javascript",           // xml, javascript, network, storage, etc.
  "level": "error",                // verbose, info, warning, error
  "text": "Error message",         // Log message text
  "timestamp": 1234567890.123,     // Timestamp when log was created
  "url": "file://example.js",      // Optional URL associated with log
  "lineNumber": 42,                // Optional line number
  "category": "console"            // Optional log category
}
```

#### Client-side Logging System

A singleton logger is available for applications to send logs directly to DevTools:

```cpp
#include "client/logger.hpp"

// Singleton access
auto logger = logging::Logger::GetInstance();

// Convenience methods
logger->info("Application started");
logger->error("Connection failed");
logger->jsError("JavaScript error occurred");

// Or use global convenience functions
logging::LogInfo("Information message");
logging::LogJsWarn("JavaScript warning");
```

#### Example Usage

```javascript
// Enable log domain
{
  "id": 1,
  "method": "Log.enable",
  "params": {}
}

// Response
{
  "id": 1,
  "result": {}
}

// Real-time log event (sent automatically)
{
  "method": "Log.entryAdded",
  "params": {
    "entry": {
      "source": "javascript",
      "level": "error", 
      "text": "Uncaught TypeError: Cannot read property 'x' of undefined",
      "timestamp": 1234567890.123,
      "url": "file://app.js",
      "lineNumber": 25
    }
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

### DOM Domain

The DOM domain provides access to the Document Object Model, allowing CDP clients to inspect and manipulate the DOM tree structure.

#### Methods

- **`DOM.enable`** - Enables DOM domain
- **`DOM.disable`** - Disables DOM domain  
- **`DOM.getDocument`** - Returns the root DOM node with the current document

#### Node Structure

Each DOM node returned by the domain includes:
- `nodeId` - Unique identifier for the node
- `parentId` - Identifier of the parent node
- `backendNodeId` - Backend-specific node identifier
- `nodeType` - Type of the node (Document, Element, Text, etc.)
- `nodeName` - Name of the node
- `localName` - Local name for elements
- `nodeValue` - Value for text nodes
- `childNodeCount` - Number of child nodes
- `children` - Array of child nodes (when included)
- `attributes` - Array of attribute name/value pairs for elements
- `documentURL` - URL of the document (for document nodes)
- `baseURL` - Base URL of the document (for document nodes)

#### Example Usage

```javascript
// Enable DOM domain
{
  "id": 1,
  "method": "DOM.enable",
  "params": {}
}

// Get document
{
  "id": 2,
  "method": "DOM.getDocument",
  "params": {}
}

// Response
{
  "id": 2,
  "result": {
    "root": {
      "nodeId": 1,
      "parentId": 0,
      "backendNodeId": 1,
      "nodeType": 9,
      "nodeName": "#document",
      "localName": "",
      "nodeValue": "",
      "childNodeCount": 1,
      "children": [
        {
          "nodeId": 2,
          "parentId": 1,
          "backendNodeId": 2,
          "nodeType": 1,
          "nodeName": "HTML",
          "localName": "html",
          "nodeValue": "",
          "childNodeCount": 0,
          "children": [],
          "attributes": []
        }
      ],
      "attributes": [],
      "documentURL": "about:blank",
      "baseURL": "about:blank"
    }
  }
}
```

## Conditional Compilation Support

All inspector functionality can be disabled at compile time using the `TR_ENABLE_INSPECTOR` preprocessor directive:

```cpp
#ifdef TR_ENABLE_INSPECTOR
// Inspector-specific code
inspectorChanPort = config.inspectorChanPort;
// Initialize inspector IPC channels
#endif
```

When `TR_ENABLE_INSPECTOR` is not defined:
- All inspector IPC channels are excluded from compilation
- Content processes start normally without inspector support
- No inspector-related assertions or runtime overhead
- Client-side logger falls back to `std::cout`/`std::cerr` output

This allows for inspector-free builds while maintaining full core functionality.

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

### Adding New Host-Side Domains

1. Create a new domain handler class extending `CdpDomainHandler`
2. Implement the `handleMethod` function
3. Register the domain in `CdpHandler::CdpHandler()`

#### Example

```cpp
class CdpCustomDomain : public CdpDomainHandler {
public:
    std::string handleMethod(const std::string& method, const CdpMessage& message, const std::string& clientId) override {
        if (method == "customMethod") {
            rapidjson::Document result;
            result.SetObject();
            // ... build result
            return CdpResponse::success(message.id, result);
        }
        return CdpResponse::error(message.id, -32601, "Custom." + method + " is not supported");
    }
};

// Register in CdpHandler::CdpHandler()
domains_["Custom"] = std::make_unique<CdpCustomDomain>();
```

### Adding New Content-Side Domains

1. Create a new domain handler class extending `ContentCdpDomainHandler` in `src/client/inspector/`
2. Implement the `handleMethod` function
3. Register the domain in `ContentCdpHandler::ContentCdpHandler()`
4. Add the domain name to `ContentDomainProxy::setupForwardedDomains()`

#### Example

```cpp
// In src/client/inspector/content_custom_domain.hpp
class ContentCdpCustomDomain : public ContentCdpDomainHandler {
public:
    std::string handleMethod(const std::string& method, const CdpMessage& message) override {
        if (method == "customMethod") {
            rapidjson::Document result;
            result.SetObject();
            result.AddMember("processType", rapidjson::Value("content", result.GetAllocator()), result.GetAllocator());
            // ... build result
            return CdpResponse::success(message.id, result);
        }
        return CdpResponse::error(message.id, -32601, "Custom." + method + " is not supported in content process");
    }
};

// Register in ContentCdpHandler::ContentCdpHandler()
registerDomain("Custom", std::make_unique<ContentCdpCustomDomain>());

// Add to ContentDomainProxy::setupForwardedDomains()
forwardedDomains_.insert("Custom");
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

## Content Process Inspector Implementation

This section describes the CDP domain implementations that run within content processes, separate from the host/runtime process.

### Overview

The JSAR Runtime uses a multi-process architecture where:
- **Host Process**: Manages the overall runtime, renderer, and protocol handling
- **Content Processes**: Handle individual documents/web content and their JavaScript execution

The content process inspector provides CDP domain handlers that run in content processes, allowing debugging and inspection of content-specific functionality like JavaScript runtime, DOM manipulation, and content-specific features.

### Architecture

#### Files

- **`content_cdp_handler.hpp/cpp`**: Main CDP handler for content processes
  - Manages domain registration and message routing
  - Parses incoming CDP messages from the host proxy
  - Dispatches to appropriate domain handlers

- **`content_runtime_domain.hpp/cpp`**: Runtime domain implementation
  - Handles JavaScript runtime inspection (Runtime.enable, Runtime.getVersion, etc.)
  - Provides content-process-specific runtime information
  - Operates on the actual JavaScript engine within the content process

- **`content_example_domain.hpp/cpp`**: Example domain for testing
  - Demonstrates content-side CDP domain implementation
  - Provides ping/pong functionality for connectivity testing
  - Shows how to implement custom domains in content processes

- **`content_log_domain.hpp/cpp`**: Log domain implementation
  - Handles Chrome DevTools Protocol Log domain methods
  - Manages log entry collection and buffering (last 1000 entries)
  - Supports performance violations reporting
  - Sends real-time `Log.entryAdded` events to DevTools
  - Integrates with singleton logging system for application logging

#### Communication Flow

1. CDP client sends request to host process
2. Host `CdpHandler` receives request
3. `ContentDomainProxy` forwards request to content process via IPC
4. Content process `ContentCdpHandler` receives and processes request
5. Content domain handler executes the requested method
6. Response sent back through IPC to host process
7. Host process returns response to CDP client

#### Adding New Content Domains

To add a new CDP domain that runs in content processes:

1. Create domain header file (e.g., `content_my_domain.hpp`)
2. Extend `ContentCdpDomainHandler` base class
3. Implement required virtual methods
4. Create implementation file (e.g., `content_my_domain.cpp`)
5. Register domain in `ContentCdpHandler` constructor
6. Add domain name to forwarded domains in `ContentDomainProxy`

#### Key Differences from Host Domains

- **Process Context**: Run within content processes, have access to content-specific state
- **IPC Communication**: Receive requests via IPC rather than direct function calls
- **Async Nature**: All communication is asynchronous through message passing
- **Isolation**: Each content process has its own domain instances
- **Lifecycle**: Tied to content process lifecycle

This design enables better isolation, scalability, and debugging capabilities by allowing direct inspection of content process state without cross-process synchronization overhead.
