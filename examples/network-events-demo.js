/**
 * Simple demonstration of network online/offline events
 * 
 * This example shows how to use the online/offline events in JSAR
 */

// Example 1: Using addEventListener
window.addEventListener('online', function() {
  console.log('📶 Network is back online!');
});

window.addEventListener('offline', function() {
  console.log('📵 Network went offline!');
});

// Example 2: Using event properties
window.ononline = function() {
  console.log('🌐 Connected to network (via ononline property)');
};

window.onoffline = function() {
  console.log('🚫 Disconnected from network (via onoffline property)');
};

// Example 3: Checking current status
if (navigator.onLine) {
  console.log('✅ Currently online');
} else {
  console.log('❌ Currently offline');
}

// Example 4: Handling network status in application logic
function handleNetworkChange() {
  const statusElement = document.getElementById('network-status');
  
  function updateStatus() {
    if (navigator.onLine) {
      statusElement.textContent = 'Online';
      statusElement.className = 'status-online';
      // Resume network operations
      fetchData();
    } else {
      statusElement.textContent = 'Offline';
      statusElement.className = 'status-offline';
      // Use cached data or show offline message
      showOfflineMessage();
    }
  }
  
  // Set initial status
  updateStatus();
  
  // Listen for changes
  window.addEventListener('online', updateStatus);
  window.addEventListener('offline', updateStatus);
}

function fetchData() {
  // Example network operation that might fail when offline
  fetch('/api/data')
    .then(response => response.json())
    .then(data => {
      console.log('Data fetched successfully:', data);
    })
    .catch(error => {
      console.error('Failed to fetch data:', error);
      // Maybe the network went down during the request
      if (!navigator.onLine) {
        showOfflineMessage();
      }
    });
}

function showOfflineMessage() {
  console.log('💾 Using cached data or showing offline message');
  // Implementation would show cached content or offline UI
}

export { handleNetworkChange, fetchData, showOfflineMessage };