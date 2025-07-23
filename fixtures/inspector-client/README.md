# JSAR Inspector WebSocket Examples

This directory contains examples demonstrating how to connect to and interact with the JSAR Inspector's WebSocket endpoint with **Chrome DevTools Protocol (CDP)** support.

## Prerequisites

- JSAR runtime must be running with inspector enabled (build with `INSPECTOR=yes`)
- Default inspector port: 9423
- WebSocket endpoint: `ws://localhost:9423/devtools/inspector/:client` ⭐ UPDATED

**Note:** WebSocket upgrades are now restricted to the `/devtools/inspector/:client` path pattern for security and compatibility.

## Examples

### 1. Browser CDP Test Client (`inspector_websocket_test.html`) ⭐ NEW

A comprehensive browser-based client that provides full CDP testing capabilities with an intuitive interface.

**Usage:**
```bash
# Open in any modern web browser
open fixtures/inspector-client/inspector_websocket_test.html
# or
firefox fixtures/inspector-client/inspector_websocket_test.html
```

**Features:**
- 🧪 **Comprehensive Test Suite** - Run all CDP tests with one click
- 📊 **Real-time Statistics** - Track messages, tests, and connection time
- 🎯 **Domain-specific Testing** - Test Runtime, Example, and error handling separately
- 📤 **Custom Message Sender** - Send custom CDP messages with validation
- 🔗 **Template Library** - Quick access to common CDP message templates
- 📋 **Interactive Message Log** - Real-time message display with formatting
- ⚡ **No Dependencies** - Pure HTML/CSS/JavaScript, runs in any browser

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

## Chrome DevTools Protocol Support ⭐ UPDATED

The JSAR Inspector now implements basic Chrome DevTools Protocol (CDP) support, enabling standard debugging tools to connect and interact with the runtime.

### Supported Domains

#### Runtime Domain
- **Runtime.enable** - Enable runtime domain
- **Runtime.disable** - Disable runtime domain  
- **Runtime.getVersion** - Get JavaScript runtime version info

#### Example Domain (renamed from MyExample)
- **Example.ping** - Simple connectivity test
- **Example.echo** - Echo back parameters
- **Example.getInfo** - Get domain information

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
  "method": "Example.ping",
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

## WebSocket Connection Requirements ⭐ UPDATED

The inspector now enforces strict path validation for WebSocket upgrades:

- ✅ **Allowed:** `ws://localhost:9423/devtools/inspector/1`
- ✅ **Allowed:** `ws://localhost:9423/devtools/inspector/main`
- ❌ **Rejected:** `ws://localhost:9423/` (old generic endpoint)
- ❌ **Rejected:** `ws://localhost:9423/debug` (unsupported path)

The inspector enforces a maximum of 5 concurrent WebSocket connections. Additional connection attempts will receive an HTTP 503 Service Unavailable response.

## Troubleshooting

**Connection Refused:**
- Ensure JSAR runtime is running
- Build with `INSPECTOR=yes` flag: `make darwin INSPECTOR=yes`
- Verify inspector is enabled
- Check the correct port (default: 9423)

**WebSocket Upgrade Failed:**
- Ensure URL follows pattern: `/devtools/inspector/:client`
- Check that the client ID is not empty
- Verify the connection limit hasn't been reached

**CDP Messages Not Working:**
- Verify message format: `{id, method, params}`
- Check supported domains in `/json/protocol`
- Use the browser test client for comprehensive validation

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
- ✅ **Path-based WebSocket validation** ⭐ NEW
- ✅ **Browser-based testing interface** ⭐ NEW

For technical details, see `docs/cdp_support.md`.