// Sample background script for JSAR Extension System
// Executes in a dedicated forked process with full Chrome API access

console.log('Sample extension background script loaded in dedicated process!');

// Show process information
console.log('Process ID:', process.pid);
console.log('Process arguments:', process.argv);

// Access environment variables set by the extension system
console.log('Extension ID:', process.env.EXTENSION_ID);
console.log('Extension name:', process.env.EXTENSION_NAME);
console.log('Extension version:', process.env.EXTENSION_VERSION);
console.log('Extension base path:', process.env.EXTENSION_BASE_PATH);

// Demonstrate that this is a separate process context
console.log('Current working directory:', process.cwd());
console.log('Node.js version:', process.version);

// TODO: Chrome Extension APIs will be available here
// Background scripts have full API access including:
/*
if (chrome && chrome.extension) {
  console.log('Extension ID via API:', chrome.extension.id);
  console.log('Extension manifest:', chrome.extension.manifest);
}

if (chrome && chrome.tabs) {
  // Full tabs API access
  chrome.tabs.query({active: true}, (tabs) => {
    console.log('Active tabs:', tabs);
  });
}

if (chrome && chrome.storage) {
  // Storage API access
  chrome.storage.local.set({timestamp: Date.now()}, () => {
    console.log('Data stored in background process');
  });
}

// Listen for messages from content scripts
chrome.runtime.onMessage.addListener((message, sender, sendResponse) => {
  console.log('Background received message:', message);
  if (message.type === 'contentScriptLoaded') {
    console.log('Content script loaded on:', message.url);
  }
  sendResponse({received: true});
});
*/

// Demonstrate background process lifecycle
process.on('SIGTERM', () => {
  console.log('Background process received SIGTERM, cleaning up...');
  process.exit(0);
});

process.on('SIGINT', () => {
  console.log('Background process received SIGINT, cleaning up...');
  process.exit(0);
});

console.log('Sample extension background script initialization complete');
console.log('Background process will continue running until extension is unloaded');