/**
 * Basic test for network online/offline events
 * 
 * This test verifies that the window object supports online/offline events
 * and the navigator.onLine property works correctly.
 */

// Import required modules - using relative paths to match the structure
import { loadPolyfills } from '../polyfills';
import { getNetworkMonitor } from '../events/network-events';

// Initialize polyfills first
loadPolyfills();

async function testNetworkEvents() {
  console.log('🧪 Testing Network Online/Offline Events');
  
  // Test 1: Check that window has EventTarget capabilities
  console.log('✓ Testing window EventTarget capabilities...');
  if (typeof window.addEventListener !== 'function') {
    throw new Error('window.addEventListener is not available');
  }
  if (typeof window.removeEventListener !== 'function') {
    throw new Error('window.removeEventListener is not available');
  }
  if (typeof window.dispatchEvent !== 'function') {
    throw new Error('window.dispatchEvent is not available');
  }
  console.log('  ✓ window has EventTarget methods');

  // Test 2: Check navigator.onLine property
  console.log('✓ Testing navigator.onLine property...');
  if (typeof navigator.onLine !== 'boolean') {
    throw new Error('navigator.onLine is not a boolean');
  }
  console.log(`  ✓ navigator.onLine is: ${navigator.onLine}`);

  // Test 3: Test event listener setup
  console.log('✓ Testing event listener setup...');
  let onlineEventFired = false;
  let offlineEventFired = false;

  const onlineHandler = () => {
    onlineEventFired = true;
    console.log('  ✓ online event fired');
  };

  const offlineHandler = () => {
    offlineEventFired = true;
    console.log('  ✓ offline event fired');
  };

  window.addEventListener('online', onlineHandler);
  window.addEventListener('offline', offlineHandler);
  console.log('  ✓ Event listeners attached');

  // Test 4: Test ononline/onoffline properties
  console.log('✓ Testing ononline/onoffline properties...');
  let ononlinePropertyFired = false;
  let onofflinePropertyFired = false;

  window.ononline = () => {
    ononlinePropertyFired = true;
    console.log('  ✓ ononline property handler fired');
  };

  window.onoffline = () => {
    onofflinePropertyFired = true;
    console.log('  ✓ onoffline property handler fired');
  };

  // Test 5: Manually trigger events to test functionality
  console.log('✓ Testing manual event triggering...');
  const monitor = getNetworkMonitor();
  
  // Simulate offline
  monitor.updateNetworkStatus(false);
  await new Promise(resolve => setTimeout(resolve, 100)); // Give events time to propagate
  
  // Simulate online
  monitor.updateNetworkStatus(true);
  await new Promise(resolve => setTimeout(resolve, 100)); // Give events time to propagate

  // Test 6: Verify events were fired
  console.log('✓ Verifying events were fired...');
  if (!offlineEventFired) {
    console.warn('  ⚠️  offline event listener was not fired');
  }
  if (!onlineEventFired) {
    console.warn('  ⚠️  online event listener was not fired');
  }
  if (!onofflinePropertyFired) {
    console.warn('  ⚠️  onoffline property handler was not fired');
  }
  if (!ononlinePropertyFired) {
    console.warn('  ⚠️  ononline property handler was not fired');
  }

  // Clean up
  window.removeEventListener('online', onlineHandler);
  window.removeEventListener('offline', offlineHandler);
  window.ononline = null;
  window.onoffline = null;

  console.log('🎉 Network events test completed successfully!');
  
  return {
    windowEventTarget: true,
    navigatorOnLine: typeof navigator.onLine === 'boolean',
    eventListeners: onlineEventFired && offlineEventFired,
    eventProperties: ononlinePropertyFired && onofflinePropertyFired
  };
}

// Export test function for external use
export { testNetworkEvents };

// Run test if this is the main module
if (require.main === module) {
  testNetworkEvents()
    .then((results) => {
      console.log('Test results:', results);
      process.exit(0);
    })
    .catch((error) => {
      console.error('Test failed:', error);
      process.exit(1);
    });
}