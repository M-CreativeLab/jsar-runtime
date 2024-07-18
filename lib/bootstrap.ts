import 'babylonjs';
/**
 * A patch to the Node.js TextDecoder.
 *
 * Node.js TextDecoder with samll ICU doesn't support ascii encoding, however the fontkit library depends on
 * the ASCII decoder to parse the font file headers.
 */
import './nodejs-patches/textdecoder';

const isDebugMode = process.env.JSAR_DEBUG_ENABLED === 'yes';

/**
 * Rewrite the `BABYLON.ThinEngine.QueueNewFrame` to use the requester's `requestAnimationFrame` function.
 * It skips the window object check.
 */
BABYLON.ThinEngine.QueueNewFrame = (func: () => void, requester?: any): number => {
  const { requestAnimationFrame } = requester || globalThis;
  if (typeof requestAnimationFrame !== 'function') {
    throw new TypeError('requestAnimationFrame is not a function, and the requester is: ' + requester);
  }

  let animationframeCallback;
  if (isDebugMode) {
    animationframeCallback = function animationframeCallback() {
      try {
        func.apply(requester, arguments);
      } catch (err) {
        console.error(`failed to execute the queued frame:`, err);
      }
    };
  } else {
    animationframeCallback = func.bind(requester);
  }
  return requestAnimationFrame(animationframeCallback);
};

BABYLON.Logger.OnNewCacheEntry = (entry) => {
  entry.split('\n').forEach((line) => {
    console.info('[Babylonjs]', line);
  });
};
BABYLON.Tools.GetAbsoluteUrl = (url: string) => {
  console.info('[Babylonjs] GetAbsoluteUrl:', url);
  return url;
};
