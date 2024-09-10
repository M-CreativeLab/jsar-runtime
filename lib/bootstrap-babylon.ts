import 'babylonjs';
import { isDebugMode } from './bootstrap-common';

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
