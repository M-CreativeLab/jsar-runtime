#!/usr/bin/env node

/**
 * JSAR Inspector CDP Test Client
 * 
 * This enhanced client demonstrates the new Chrome DevTools Protocol (CDP) 
 * support in the JSAR Inspector. It tests both Runtime and MyExample domains.
 * 
 * Usage:
 *   node fixtures/inspector-client/inspector_cdp_test_client.js
 * 
 * Prerequisites:
 *   - JSAR runtime must be running with inspector enabled (INSPECTOR=yes)
 *   - Default inspector port is 9423
 */

const WebSocket = require('ws');

// Inspector WebSocket endpoint
const INSPECTOR_URL = 'ws://localhost:9423/devtools/inspector/1';

// CDP test messages
const testMessages = [
    {
        name: 'Runtime.enable',
        message: {
            id: 1,
            method: 'Runtime.enable',
            params: {}
        }
    },
    {
        name: 'Runtime.getVersion',
        message: {
            id: 2,
            method: 'Runtime.getVersion',
            params: {}
        }
    },
    {
        name: 'MyExample.ping',
        message: {
            id: 3,
            method: 'MyExample.ping',
            params: {}
        }
    },
    {
        name: 'MyExample.echo',
        message: {
            id: 4,
            method: 'MyExample.echo',
            params: {
                message: 'Hello JSAR CDP!',
                timestamp: Date.now(),
                testData: { nested: true, array: [1, 2, 3] }
            }
        }
    },
    {
        name: 'MyExample.getInfo',
        message: {
            id: 5,
            method: 'MyExample.getInfo',
            params: {}
        }
    },
    {
        name: 'Unknown Domain Test',
        message: {
            id: 6,
            method: 'UnknownDomain.test',
            params: {}
        }
    },
    {
        name: 'Unknown Method Test',
        message: {
            id: 7,
            method: 'Runtime.unknownMethod',
            params: {}
        }
    }
];

// Create WebSocket connection
const ws = new WebSocket(INSPECTOR_URL);
let currentTestIndex = 0;
let testResults = [];

// Connection opened
ws.on('open', function open() {
    console.log('✓ Connected to JSAR Inspector WebSocket with CDP support');
    console.log('  URL:', INSPECTOR_URL);
    console.log('\n🧪 Starting CDP Test Suite...\n');
    
    // Start testing
    sendNextTest();
});

// Message received
ws.on('message', function message(data) {
    const dataStr = data.toString();
    console.log('← Received CDP response:');
    
    try {
        const response = JSON.parse(dataStr);
        console.log(JSON.stringify(response, null, 2));
        
        // Record test result
        const currentTest = testMessages[currentTestIndex - 1];
        if (currentTest) {
            testResults.push({
                test: currentTest.name,
                success: validateResponse(currentTest, response),
                response: response
            });
        }
        
        console.log(''); // Add spacing
        
        // Send next test after a delay
        setTimeout(() => {
            sendNextTest();
        }, 1000);
        
    } catch (error) {
        console.log('Raw response:', dataStr);
        console.log('Parse error:', error.message);
        
        // Continue with next test
        setTimeout(() => {
            sendNextTest();
        }, 1000);
    }
});

// Connection closed
ws.on('close', function close(code, reason) {
    console.log('\n✗ Connection closed');
    console.log('  Code:', code);
    console.log('  Reason:', reason.toString());
    
    // Print test summary
    printTestSummary();
});

// Connection error
ws.on('error', function error(err) {
    console.error('\n✗ WebSocket error:', err.message);
    
    if (err.code === 'ECONNREFUSED') {
        console.error('\nTroubleshooting:');
        console.error('  • Make sure JSAR runtime is running');
        console.error('  • Build with INSPECTOR=yes flag');
        console.error('  • Verify the port (default: 9423)');
        console.error('  • Check if content is loaded (content ID: 1)');
    }
});

function sendNextTest() {
    if (currentTestIndex >= testMessages.length) {
        // All tests completed
        console.log('🎉 All CDP tests completed!\n');
        ws.close();
        return;
    }
    
    const test = testMessages[currentTestIndex];
    currentTestIndex++;
    
    console.log(`→ Test ${currentTestIndex}: ${test.name}`);
    console.log('Sending:', JSON.stringify(test.message, null, 2));
    
    ws.send(JSON.stringify(test.message));
}

function validateResponse(test, response) {
    // Basic validation - check if we got a proper CDP response
    if (!response || typeof response !== 'object') {
        return false;
    }
    
    // Should have either result or error
    if (!('result' in response) && !('error' in response)) {
        return false;
    }
    
    // Should have matching ID (except for parse errors)
    if (response.id !== test.message.id && response.id !== -1) {
        return false;
    }
    
    // Check specific expectations
    if (test.name.includes('Unknown')) {
        // Should be an error response
        return 'error' in response && response.error.code === -32601;
    } else {
        // Should be a success response
        return 'result' in response;
    }
}

function printTestSummary() {
    console.log('\n📊 Test Summary');
    console.log('================');
    
    let passed = 0;
    let total = testResults.length;
    
    testResults.forEach((result, index) => {
        const status = result.success ? '✅' : '❌';
        console.log(`${index + 1}. ${status} ${result.test}`);
        if (result.success) passed++;
    });
    
    console.log(`\nResults: ${passed}/${total} tests passed`);
    
    if (passed === total) {
        console.log('🎉 All tests passed! CDP implementation is working correctly.');
    } else {
        console.log('⚠️  Some tests failed. Check the responses above for details.');
    }
    
    console.log('\n💡 Next Steps:');
    console.log('• Try connecting with Chrome DevTools');
    console.log('• Use the browser test page: fixtures/inspector-client/inspector_websocket_test.html');
    console.log('• Check the protocol at: http://localhost:9423/json/protocol');
    console.log('• View available targets at: http://localhost:9423/json/list');
}

// Handle process termination
process.on('SIGINT', function() {
    console.log('\n\nClosing WebSocket connection...');
    ws.close();
    process.exit(0);
});

console.log('JSAR Inspector CDP Test Client');
console.log('==============================');
console.log('Testing Chrome DevTools Protocol support');
console.log('Connecting to:', INSPECTOR_URL);
console.log('Press Ctrl+C to exit\n');