// Network Events Demo
// This example shows how to use the network online/offline events in JSAR

// Method 1: Using event handler properties
window.ononline = function() {
  console.log('📶 Network is back online!');
  
  // Resume network operations
  document.getElementById('status').textContent = 'Online';
  document.getElementById('status').style.color = 'green';
  
  // Re-enable network-dependent features
  enableNetworkFeatures();
};

window.onoffline = function() {
  console.log('📵 Network went offline!');
  
  // Switch to offline mode
  document.getElementById('status').textContent = 'Offline';
  document.getElementById('status').style.color = 'red';
  
  // Disable network-dependent features
  disableNetworkFeatures();
};

// Method 2: Check current network status
function checkNetworkStatus() {
  if (navigator.onLine) {
    console.log('✅ Currently online');
    return true;
  } else {
    console.log('❌ Currently offline');
    return false;
  }
}

// Example functions for handling network state
function enableNetworkFeatures() {
  // Re-enable sync, live updates, etc.
  document.querySelectorAll('.network-feature').forEach(element => {
    element.disabled = false;
  });
}

function disableNetworkFeatures() {
  // Disable features that require network
  document.querySelectorAll('.network-feature').forEach(element => {
    element.disabled = true;
  });
}

// Initialize the app
function initializeApp() {
  // Check initial network status
  checkNetworkStatus();
  
  // Set up the UI
  const statusElement = document.getElementById('status');
  if (navigator.onLine) {
    statusElement.textContent = 'Online';
    statusElement.style.color = 'green';
    enableNetworkFeatures();
  } else {
    statusElement.textContent = 'Offline';
    statusElement.style.color = 'red';
    disableNetworkFeatures();
  }
}

// Run when the page loads
document.addEventListener('DOMContentLoaded', initializeApp);

// Example: Sync data when coming back online
window.ononline = function() {
  console.log('🔄 Syncing data after reconnection...');
  
  // Sync pending changes
  syncPendingData()
    .then(() => console.log('✅ Sync completed'))
    .catch(err => console.error('❌ Sync failed:', err));
};

async function syncPendingData() {
  // Example implementation
  const pendingItems = getPendingItems();
  
  for (const item of pendingItems) {
    try {
      await uploadItem(item);
      markItemAsSynced(item);
    } catch (error) {
      console.error('Failed to sync item:', item, error);
    }
  }
}

function getPendingItems() {
  // Return items that need to be synced
  return JSON.parse(localStorage.getItem('pendingItems') || '[]');
}

async function uploadItem(item) {
  // Upload item to server
  const response = await fetch('/api/items', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(item)
  });
  
  if (!response.ok) {
    throw new Error(`Upload failed: ${response.statusText}`);
  }
  
  return response.json();
}

function markItemAsSynced(item) {
  // Remove from pending items
  const pendingItems = getPendingItems();
  const updatedItems = pendingItems.filter(p => p.id !== item.id);
  localStorage.setItem('pendingItems', JSON.stringify(updatedItems));
}