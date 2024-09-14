console.info('worker.js loaded');

onmessage = function(e) {
  console.info('worker.js received message:', e.data);
};
