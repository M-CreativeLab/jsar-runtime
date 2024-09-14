console.info('worker.js loaded');

onmessage = function(e) {
  if (e.data.text === 'foo') {
    postMessage('bar');
  }
};
