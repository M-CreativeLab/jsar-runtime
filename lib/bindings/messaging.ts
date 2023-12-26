import * as logger from './logger';

let nativeListener = null;
const eventTarget = new EventTarget();

function onUnityEvent(type: string, message: string) {
  /**
   * This function is called from native side, we should make sure this function without any error to avoid native crash.
   */
  try {
    const newEvent = new CustomEvent(type, { detail: message });
    eventTarget.dispatchEvent(newEvent);
  } catch (err) {
    logger.warn(`failed to process event "${type}" with message "${message}", the error is: ${err}`);
  }
}

try {
  const binding = (process as any)._linkedBinding('transmute:messaging');
  nativeListener = new binding.UnityEventListener();
  nativeListener.setCallback(onUnityEvent);
  console.info('loaded linked messaging module and set the callback ready.');
} catch (err) {
  console.warn('failed to load linked messaging module, switch to console instead.');
}

export function addEventListener(
  type: string,
  callback: EventListenerOrEventListenerObject,
  options?: boolean | AddEventListenerOptions
) {
  return eventTarget.addEventListener(type, callback, options);
}

export const removeEventListener = eventTarget.removeEventListener.bind(eventTarget);
export const dispatchEvent = eventTarget.dispatchEvent.bind(eventTarget);

export const emit = function emitMessagingEvent(type: string, message?: string) {
  if (typeof type !== 'string') {
    throw new TypeError('event type must be a string');
  }
  return eventTarget.dispatchEvent(new CustomEvent(type, { detail: message }));
};

/**
 * Dispose the native listener, this drops the reference of the low-level tsfn instance.
 */
export const dispose = () => {
  if (typeof nativeListener?.dispose === 'function') {
    nativeListener.dispose();
  }
};
