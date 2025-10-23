console.info('worker.js loaded');

onmessage = function(e) {
  if (e.data.text === 'foo') {
    postMessage({
      text: 'bar',
      buffer: Float32Array.from([1, 2, 3])
    });
  }
};
