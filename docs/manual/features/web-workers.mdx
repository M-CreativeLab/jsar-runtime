# Web Workers

__Web Workers__ allows you to run JavaScript code in a separate thread, which is useful for offloading heavy tasks from the main thread. This can be useful for tasks like physics calculations, AI, or other heavy computations.

If you want to learn more about Web Workers, you can read the [MDN Web Docs](https://developer.mozilla.org/en-US/docs/Web/API/Web_Workers_API).

## Implementation status

__Web Workers__ are supported in JSAR but not completely, the following are the implemented Worker types:

- [x] __Dedicated Workers__ are workers that are utilized by a single script, and the context is also dedicated to that script.
- [ ] __Shared Workers__ are workers that can be utilized by multiple scripts running in different windows, IFrames, etc., as long as they are in the same domain as the worker. They are a little more complex than dedicated workers — scripts must communicate via an active port.
- [ ] __Service Workers__ essentially act as proxy servers that sit between web applications, and the browser and the network (when available). They are intended to help create effective offline experiences, intercept network requests, and take appropriate action based on whether the network is available.

## Getting started

To create a new Worker, you first need an entry point file for the worker:

```js title="worker.js"
console.info('worker.js loaded');

onmessage = function(e) {
  if (e.data.text === 'foo') {
    postMessage('bar');
  }
};
```

Then, you can create a new `Worker` instance in your main script:

```html title="index.html"
<html>
<head>
  <meta charset="utf-8" />
  <title>Web Workers</title>
  <script>
    const worker = new Worker('worker.js');
    worker.onmessage = (event) => {
      console.log('Received message from worker:', event.data);
    };
    worker.postMessage({ text: 'foo' });
  </script>
</head>
<body>
</body>
</html>
```

After opening the `index.html` file in JSAR, you should see the message `Received message from worker: bar` in the console.

## Supported Web APIs

The following Web APIs are supported in Web Workers:

- Objects
  - [x] [`console`](https://developer.mozilla.org/en-US/docs/Web/API/Console)
- Classes
  - [x] [`URL`](https://developer.mozilla.org/en-US/docs/Web/API/URL)
  - [x] [`Blob`](https://developer.mozilla.org/en-US/docs/Web/API/Blob)
  - [x] [`TextDecoder`](https://developer.mozilla.org/en-US/docs/Web/API/TextDecoder)
  - [x] [`OffscreenCanvas`](https://developer.mozilla.org/en-US/docs/Web/API/OffscreenCanvas)
  - [x] [`Worker`](https://developer.mozilla.org/en-US/docs/Web/API/Worker)
  - [x] [`Headers`](https://developer.mozilla.org/en-US/docs/Web/API/Headers)
  - [x] [`Request`](https://developer.mozilla.org/en-US/docs/Web/API/Request)
  - [x] [`Response`](https://developer.mozilla.org/en-US/docs/Web/API/Response)
- Global functions
  - [x] [`atob`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/atob)
  - [x] [`btob`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/btoa)
  - [x] [`fetch`](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API)
  - [x] [`setTimeout`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/setTimeout)
  - [x] [`clearTimeout`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/clearTimeout)
  - [x] [`setInterval`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/setInterval)
  - [x] [`clearInterval`](https://developer.mozilla.org/en-US/docs/Web/API/WindowOrWorkerGlobalScope/clearInterval)
  - [x] [`postMessage`](https://developer.mozilla.org/en-US/docs/Web/API/Worker/postMessage)
