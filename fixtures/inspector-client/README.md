# JSAR Inspector WebSocket Examples

This directory contains examples demonstrating how to connect to and interact with the JSAR Inspector's WebSocket endpoint with **Chrome DevTools Protocol (CDP)** support.

## Prerequisites

- JSAR runtime must be running with inspector enabled (build with `INSPECTOR=yes`)
- Default inspector port: 9423
- WebSocket endpoint: `ws://localhost:9423/devtools/inspector/1`

## Examples

### 1. Node.js CDP Test Client (`inspector_cdp_test_client.js`) ⭐ NEW

A comprehensive Node.js client that tests the new Chrome DevTools Protocol support.

**Usage:**
```bash
# Run the CDP test client
node fixtures/inspector-client/inspector_cdp_test_client.js
```

**Features:**
- Tests all supported CDP domains (Runtime, MyExample)
- Validates CDP message format compliance
- Tests error handling for unknown domains/methods
- Provides detailed test results and validation
- Demonstrates proper CDP usage patterns

### 2. Node.js Basic Client (`inspector_websocket_client.js`)

A simple command-line Node.js client for basic WebSocket connection testing.

**Usage:**
```bash
# Install dependencies (if not already installed)
npm install ws

# Run the basic client
node fixtures/inspector-client/inspector_websocket_client.js
```

**Features:**
- Basic WebSocket connection testing
- Sends sample CDP messages
- Works with both old echo mode and new CDP mode

### 3. Browser Test Page (`inspector_websocket_test.html`)

An interactive HTML page for testing WebSocket connections from a browser.

**Usage:**
```bash
# Open in any modern web browser
open fixtures/inspector-client/inspector_websocket_test.html
# or
firefox fixtures/inspector-client/inspector_websocket_test.html
```

**Features:**
- Interactive WebSocket connection management
- Send custom JSON messages
- Predefined CDP message templates
- Real-time message display
- Connection status monitoring

## Chrome DevTools Protocol Support ⭐ NEW

The JSAR Inspector now implements basic Chrome DevTools Protocol (CDP) support, enabling standard debugging tools to connect and interact with the runtime.

### Supported Domains

#### Runtime Domain
- **Runtime.enable** - Enable runtime domain
- **Runtime.disable** - Disable runtime domain  
- **Runtime.getVersion** - Get JavaScript runtime version info

#### MyExample Domain (for testing)
- **MyExample.ping** - Simple connectivity test
- **MyExample.echo** - Echo back parameters
- **MyExample.getInfo** - Get domain information

### Example CDP Messages

**Runtime Enable:**
```json
{
  "id": 1,
  "method": "Runtime.enable",
  "params": {}
}
```

**Runtime Version:**
```json
{
  "id": 2,
  "method": "Runtime.getVersion",
  "params": {}
}
```

**Example Test:**
```json
{
  "id": 3,
  "method": "MyExample.ping",
  "params": {}
}
```

### Protocol Discovery

Check available domains and methods:
```bash
curl http://localhost:9423/json/protocol
```

List debug targets:
```bash
curl http://localhost:9423/json/list
```

## Connection Limits

The inspector enforces a maximum of 5 concurrent WebSocket connections. Additional connection attempts will receive an HTTP 503 Service Unavailable response.

## Troubleshooting

**Connection Refused:**
- Ensure JSAR runtime is running
- Build with `INSPECTOR=yes` flag: `make darwin INSPECTOR=yes`
- Verify inspector is enabled
- Check the correct port (default: 9423)

**CDP Messages Not Working:**
- Verify message format: `{id, method, params}`
- Check supported domains in `/json/protocol`
- Use the CDP test client for validation

**Connection Limit Reached:**
- Close existing connections
- Check for zombie connections
- Restart the inspector if necessary

## Chrome DevTools Integration

To connect with Chrome DevTools:

1. Get the DevTools URL from: `http://localhost:9423/json/list`
2. Copy the `devtoolsFrontendUrl` value
3. Open the URL in Chrome
4. Use the Console and other DevTools features

## Development Notes

The WebSocket implementation serves as the transport layer for Chrome DevTools Protocol (CDP) integration. The inspector now supports:

- ✅ Standard CDP message format
- ✅ Domain-based method organization  
- ✅ Proper error handling with JSON-RPC codes
- ✅ Extensible architecture for new domains
- ✅ Protocol discovery and introspection
- ✅ Comprehensive logging for debugging

For technical details, see `docs/cdp_support.md`.