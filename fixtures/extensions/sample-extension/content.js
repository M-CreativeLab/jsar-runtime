// Sample content script for JSAR Extension System
// Executes in the page's renderer process with DOM access

console.log('Sample extension content script loaded!');
console.log('Current page URL:', window.location.href);
console.log('Page title:', document.title);

// Demonstrate DOM manipulation
if (document.body) {
  console.log('Document body found, can manipulate DOM');
  
  // Add a small indicator that the extension is active
  const indicator = document.createElement('div');
  indicator.id = 'jsar-extension-indicator';
  indicator.style.cssText = `
    position: fixed;
    top: 10px;
    right: 10px;
    background: #4CAF50;
    color: white;
    padding: 5px 10px;
    border-radius: 3px;
    font-size: 12px;
    z-index: 999999;
    font-family: Arial, sans-serif;
  `;
  indicator.textContent = 'JSAR Extension Active';
  document.body.appendChild(indicator);
  
  // Remove indicator after 3 seconds
  setTimeout(() => {
    const element = document.getElementById('jsar-extension-indicator');
    if (element) {
      element.remove();
    }
  }, 3000);
}

// Listen for page changes (for SPA support)
if (window.history && window.history.pushState) {
  const originalPushState = window.history.pushState;
  window.history.pushState = function(...args) {
    console.log('Content script detected navigation:', args[2]);
    originalPushState.apply(window.history, args);
  };
}

// TODO: Limited Chrome API access for content scripts
// Content scripts will have restricted API access compared to background scripts
/*
if (chrome && chrome.runtime) {
  console.log('Chrome runtime available in content script');
  
  // Send message to background script
  chrome.runtime.sendMessage({
    type: 'contentScriptLoaded',
    url: window.location.href,
    title: document.title
  });
}
*/

console.log('Sample extension content script initialization complete');