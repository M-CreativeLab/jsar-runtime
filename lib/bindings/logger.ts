let nodejsUtilModule;
let binding = null;

export class Logger extends EventTarget implements Console {
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

  assert(condition?: boolean, ...data: any[]): void;
  assert(value: any, message?: string, ...optionalParams: any[]): void;
  assert(value?: unknown, message?: unknown, ...optionalParams: unknown[]): void {
    if (!value) {
      this.error(message, ...optionalParams);
    }
  }
  clear(): void {
    // TODO: clear the console.
  }
  count(label?: string): void {
    throw new TypeError('Method not implemented.');
  }
  countReset(label?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  dir(item?: any, options?: any): void;
  dir(obj?: unknown, options?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  dirxml(...data: any[]): void;
  dirxml(...data: any[]): void;
  dirxml(...data: unknown[]): void {
    throw new TypeError('Method not implemented.');
  }
  group(...label: any[]): void;
  group(...label: unknown[]): void {
    throw new TypeError('Method not implemented.');
  }
  groupCollapsed(...label: any[]): void;
  groupCollapsed(...label: unknown[]): void {
    throw new TypeError('Method not implemented.');
  }
  groupEnd(): void {
    throw new TypeError('Method not implemented.');
  }
  table(tabularData?: any, properties?: string[]): void;
  table(tabularData: any, properties?: readonly string[]): void;
  table(tabularData?: unknown, properties?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  time(label?: string): void;
  time(label?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  timeEnd(label?: string): void;
  timeEnd(label?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  timeLog(label?: string, ...data: any[]): void;
  timeLog(label?: unknown, ...data: unknown[]): void {
    throw new TypeError('Method not implemented.');
  }
  timeStamp(label?: string): void;
  timeStamp(label?: unknown): void {
    throw new TypeError('Method not implemented.');
  }
  trace(...data: any[]): void;
  trace(message?: any, ...optionalParams: any[]): void;
  trace(message?: unknown, ...optionalParams: unknown[]): void {
    throw new TypeError('Method not implemented.');
  }
  get Console(): console.ConsoleConstructor {
    throw new TypeError('Method not implemented.');
  }
  profile(label?: string): void {
    throw new TypeError('Method not implemented.');
  }
  profileEnd(label?: string): void {
    throw new TypeError('Method not implemented.');
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
