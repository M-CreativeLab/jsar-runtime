// Example usage of the Notification API in JavaScript

// Check if Notification is available
if (typeof Notification !== 'undefined') {
  console.log('Notification API is available');
  console.log('Current permission:', Notification.permission);

  // Request permission
  const permission = Notification.requestPermission();
  console.log('Permission after request:', permission);

  // Create a simple notification
  const notification1 = new Notification('Hello World!');

  // Create a notification with options
  const notification2 = new Notification('New Message', {
    body: 'You have a new message from John',
    icon: '/images/message-icon.png',
    tag: 'message-notification',
    requireInteraction: false,
    silent: false
  });

  // Set event handlers
  notification2.onshow = function() {
    console.log('Notification shown');
  };

  notification2.onclick = function() {
    console.log('Notification clicked');
    notification2.close();
  };

  notification2.onclose = function() {
    console.log('Notification closed');
  };

  notification2.onerror = function() {
    console.error('Notification error');
  };

  // Close notification after 5 seconds
  setTimeout(() => {
    notification2.close();
  }, 5000);
} else {
  console.error('Notification API is not available');
}
