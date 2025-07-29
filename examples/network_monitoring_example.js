// Example JavaScript code showing how to use the network monitoring features
// This would be used in a JSAR application

// Listen for online/offline events
window.addEventListener('online', function() {
  console.log('Network is now online');
});

window.addEventListener('offline', function() {
  console.log('Network is now offline');
});

// Check current network status
if (window._navigatorOnLine) {
  console.log('Currently online');
} else {
  console.log('Currently offline');
}

// Example: Retry failed requests when coming back online
let failedRequests = [];

window.addEventListener('offline', function() {
  console.log('Gone offline - queueing requests');
});

window.addEventListener('online', function() {
  console.log('Back online - retrying failed requests');
  failedRequests.forEach(request => {
    // Retry the request
    console.log('Retrying request:', request);
  });
  failedRequests = [];
});

function makeRequest(url) {
  if (!window._navigatorOnLine) {
    failedRequests.push(url);
    console.log('Offline - queuing request for later:', url);
    return;
  }
  
  // Make the actual request
  console.log('Making request:', url);
}