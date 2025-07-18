# JSAR Inspector WebSocket Examples

This directory contains examples demonstrating how to connect to and interact with the JSAR Inspector's WebSocket endpoint.

## Prerequisites

- JSAR runtime must be running with inspector enabled
- Default inspector port: 9423
- WebSocket endpoint: `ws://localhost:9423/devtools/inspector/1`

## Examples

### 1. Node.js Client (`inspector_websocket_client.js`)

A command-line Node.js client that demonstrates WebSocket connection and message exchange.

**Usage:**
```bash
# Install dependencies (if not already installed)
npm install ws

# Run the client
node fixtures/inspector-client/inspector_websocket_client.js
```

**Features:**
- Automatic connection to inspector WebSocket
- Sends test messages
- Displays received responses
- Error handling and troubleshooting tips

### 2. Browser Test Page (`inspector_websocket_test.html`)

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
- Predefined message templates
- Real-time message display
- Connection status monitoring

## WebSocket Protocol

The JSAR Inspector implements WebSocket support for real-time debugging communication. Currently, messages are echoed back as a placeholder for future Chrome DevTools Protocol (CDP) implementation.

### Example Messages

**Runtime Enable:**
```json
{
  "id": 1,
  "method": "Runtime.enable",
  "params": {}
}
```

**Test Message:**
```json
{
  "id": 2,
  "method": "Test.ping",
  "params": {
    "message": "Hello JSAR Inspector!"
  }
}
```

## Connection Limits

The inspector enforces a maximum of 5 concurrent WebSocket connections. Additional connection attempts will receive an HTTP 503 Service Unavailable response.

## Troubleshooting

**Connection Refused:**
- Ensure JSAR runtime is running
- Verify inspector is enabled
- Check the correct port (default: 9423)

**Connection Limit Reached:**
- Close existing connections
- Check for zombie connections
- Restart the inspector if necessary

## Development Notes

This WebSocket implementation serves as the transport layer foundation for Chrome DevTools Protocol (CDP) integration. The current echo functionality will be replaced with actual CDP message routing and domain implementations in future versions.