// Sample Extension Background Script
console.log('🎉 Sample Extension loaded successfully!');

// Extension information logging
if (chrome && chrome.extension) {
  console.log('Extension ID:', chrome.extension.id);
  console.log('Extension name:', chrome.extension.manifest.name);
  console.log('Extension version:', chrome.extension.manifest.version);
} else {
  console.log('Extension API available:', !!chrome);
}

// Example of runtime API usage
if (chrome && chrome.runtime) {
  // Simulate startup listener
  if (chrome.runtime.onStartup) {
    chrome.runtime.onStartup.addListener(() => {
      console.log('Extension startup event triggered');
    });
  }
  
  // Get extension URL example
  if (chrome.runtime.getURL) {
    const iconUrl = chrome.runtime.getURL('icon.png');
    console.log('Extension icon URL:', iconUrl);
  }
}

// Set a global flag to indicate successful loading
globalThis.sampleExtensionReady = true;

console.log('✅ Sample extension background script execution completed');