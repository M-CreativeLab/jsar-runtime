let nodejsUtilModule;
let binding = null;

export class Logger extends EventTarget {
  #getMessageString(format: string, ...args: any[]): string | string[] {
    if (typeof process === 'object' && process != null && typeof process.pid === 'number') {
      // Node.js
      if (nodejsUtilModule == null) {
        nodejsUtilModule = require('util');
      }
      return nodejsUtilModule.format(format, ...args) as string;
    } else {
      return [format].concat(args).join(' ');
    }
  }
  #writeStdout(format, ...args) {
    const message = this.#getMessageString(format, ...args);
    if (binding != null) {
      binding.writeStdout(message);
    }
    console.info(format, ...args);
    this.dispatchEvent(new CustomEvent('log', { detail: message }));
  }
  #writeStderr(format, ...args) {
    const message = this.#getMessageString(format, ...args);
    if (binding != null) {
      binding.writeStderr(message);
    }
    console.info(format, ...args);
    this.dispatchEvent(new CustomEvent('log', { detail: message }));
  }

  log(format, ...args) {
    return this.#writeStdout(format, ...args);
  }
  info(format, ...args) {
    return this.#writeStdout(format, ...args);
  }
  debug(format, ...args) {
    return this.#writeStdout(format, ...args);
  }
  warn(format, ...args) {
    return this.#writeStderr(format, ...args);
  }
  error(format, ...args) {
    return this.#writeStderr(format, ...args);
  }
}

export function getLinkedBinding() {
  if (binding == null) {
    binding = (process as any)._linkedBinding('transmute:logger');
  }
  return binding;
}

// TODO: add a linked runtime flag to instead of this try-catch block.
try {
  getLinkedBinding();
} catch (err) {
  console.warn('failed to load linked logger module, switch to console instead.');
}

const defaultLogger = new Logger();

export const log = defaultLogger.log.bind(defaultLogger);
export const info = defaultLogger.info.bind(defaultLogger);
export const debug = defaultLogger.debug.bind(defaultLogger);
export const warn = defaultLogger.warn.bind(defaultLogger);
export const error = defaultLogger.error.bind(defaultLogger);

export function getLogger() {
  return defaultLogger;
}
