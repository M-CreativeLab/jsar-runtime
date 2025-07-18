#!/usr/bin/env node

/**
 * JSAR Inspector WebSocket Client Example
 * 
 * This example demonstrates how to connect to the JSAR Inspector's WebSocket
 * endpoint and send/receive messages for debugging purposes.
 * 
 * Usage:
 *   node examples/inspector_websocket_client.js
 * 
 * Prerequisites:
 *   - JSAR runtime must be running with inspector enabled
 *   - Default inspector port is 9423
 */

const WebSocket = require('ws');

// Inspector WebSocket endpoint
const INSPECTOR_URL = 'ws://localhost:9423/devtools/inspector/1';

// Create WebSocket connection
const ws = new WebSocket(INSPECTOR_URL);

// Connection opened
ws.on('open', function open() {
  console.log('✓ Connected to JSAR Inspector WebSocket');
  console.log('  URL:', INSPECTOR_URL);
  
  // Send a test message (currently echoed back)
  const testMessage = {
    id: 1,
    method: 'Runtime.enable',
    params: {}
  };
  
  console.log('\n→ Sending test message:');
  console.log(JSON.stringify(testMessage, null, 2));
  
  ws.send(JSON.stringify(testMessage));
});

// Message received
ws.on('message', function message(data) {
  console.log('\n← Received message:');
  try {
    const parsed = JSON.parse(data.toString());
    console.log(JSON.stringify(parsed, null, 2));
  } catch (error) {
    console.log('Raw message:', data.toString());
  }
});

// Connection closed
ws.on('close', function close(code, reason) {
  console.log('\n✗ Connection closed');
  console.log('  Code:', code);
  console.log('  Reason:', reason.toString());
});

// Connection error
ws.on('error', function error(err) {
  console.error('\n✗ WebSocket error:', err.message);
  
  if (err.code === 'ECONNREFUSED') {
    console.error('\nTroubleshooting:');
    console.error('  • Make sure JSAR runtime is running');
    console.error('  • Check if inspector is enabled');
    console.error('  • Verify the port (default: 9423)');
  }
});

// Handle process termination
process.on('SIGINT', function() {
  console.log('\n\nClosing WebSocket connection...');
  ws.close();
  process.exit(0);
});

console.log('JSAR Inspector WebSocket Client');
console.log('===============================');
console.log('Connecting to:', INSPECTOR_URL);
console.log('Press Ctrl+C to exit\n');