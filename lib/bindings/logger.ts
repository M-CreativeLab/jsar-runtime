import { inspect } from 'util';
const { log: writeLogToNative } = process._linkedBinding('transmute:logger');

let nodejsUtilModule;
enum LogLevel {
  INFO = 0,
  ERROR = 1,
  WARN = 2,
}

const kSecond = 1000;
const kMinute = 60 * kSecond;
const kHour = 60 * kMinute;
const kMaxGroupIndentation = 1000;

function pad(value: number | string): string {
  return `${value}`.padStart(2, '0');
}

function formatTime(ms: number) {
  let hours = 0;
  let minutes = 0;
  let seconds = 0;

  if (ms >= kSecond) {
    if (ms >= kMinute) {
      if (ms >= kHour) {
        hours = Math.floor(ms / kHour);
        ms = ms % kHour;
      }
      minutes = Math.floor(ms / kMinute);
      ms = ms % kMinute;
    }
    seconds = ms / kSecond;
  }

  if (hours !== 0 || minutes !== 0) {
    const strs = seconds.toFixed(3).split('.');
    const res = hours !== 0 ? `${hours}:${pad(minutes)}` : minutes;
    return `${res}:${pad(strs[0])}.${strs[1]} (${hours !== 0 ? 'h:m' : ''}m:ss.mmm)`;
  }

  if (seconds !== 0) {
    return `${seconds.toFixed(3)}s`;
  }
  return `${ms.toFixed(3)}ms`;
}

export class Logger extends EventTarget implements Console {
  #times = new Map<string, [number, number]>();

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

  #write(level: LogLevel, format, ...args) {
    const message = this.#getMessageString(format, ...args);

    console.info(format, ...args);
    writeLogToNative(level, Array.isArray(message) ? message.join(' ') : message);
    this.dispatchEvent(new CustomEvent('log', { detail: message }));
  }

  #timeLogImpl(name: string, label: string, data?): boolean {
    const time = this.#times.get(label);
    if (time === undefined) {
      process.emitWarning(`No such label '${label}' for console.${name}()`);
      return false;
    }
    const duration = process.hrtime(time);
    const ms = duration[0] * 1000 + duration[1] / 1e6;

    const formatted = formatTime(ms);
    if (data === undefined) {
      this.log('%s: %s', label, formatted);
    } else {
      this.log('%s: %s', label, formatted, ...data);
    }
    return true;
  }

  log(format, ...args) {
    this.#write(LogLevel.INFO, format, ...args);
  }
  info(format, ...args) {
    this.#write(LogLevel.INFO, format, ...args);
  }
  debug(format, ...args) {
    this.#write(LogLevel.INFO, format, ...args);
  }
  warn(format, ...args) {
    this.#write(LogLevel.WARN, format, ...args);
  }
  error(format, ...args) {
    this.#write(LogLevel.ERROR, format, ...args);
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
    console.warn('The method "Logger.prototype.count()" not implemented.');
  }
  countReset(label?: unknown): void {
    console.warn('The method "Logger.prototype.countReset()" not implemented.');
  }
  dir(item?: any, options?: any): void;
  dir(obj?: unknown, options?: any): void {
    this.#write(
      LogLevel.INFO,
      inspect(obj, {
        customInspect: false,
        ...options,
      })
    );
  }
  dirxml(...data: any[]): void {
    this.log.apply(this, data);
  }
  group(...label: any[]): void;
  group(...label: unknown[]): void {
    console.warn('The method "Logger.prototype.group()" not implemented.');
  }
  groupCollapsed(...label: any[]): void;
  groupCollapsed(...label: unknown[]): void {
    console.warn('The method "Logger.prototype.groupCollapsed()" not implemented.');
  }
  groupEnd(): void {
    console.warn('The method "Logger.prototype.groupEnd()" not implemented.');
  }
  table(tabularData?: any, properties?: string[]): void;
  table(tabularData: any, properties?: readonly string[]): void;
  table(tabularData?: unknown, properties?: unknown): void {
    console.warn('The method "Logger.prototype.table()" not implemented.');
  }
  time(label: string = 'default'): void {
    // Coerces everything other than Symbol to a string
    label = `${label}`;
    if (this.#times.has(label)) {
      process.emitWarning(`Label '${label}' already exists for console.time()`);
      return;
    }
    this.#times.set(label, process.hrtime());
  }
  timeEnd(label: string = 'default'): void {
    // Coerces everything other than Symbol to a string
    label = `${label}`;
    const found = this.#timeLogImpl('timeEnd', label);
    if (found) {
      this.#times.delete(label);
    }
  }
  timeLog(label: string = 'default', ...data: any[]): void {
    // Coerces everything other than Symbol to a string
    label = `${label}`;
    this.#timeLogImpl('timeLog', label, data);
  }
  timeStamp(_label?: string): void {
    console.warn('The method "Logger.prototype.timeStamp()" not implemented.');
  }
  trace(...data: any[]): void;
  trace(message?: any, ...optionalParams: any[]): void;
  trace(message?: unknown, ...optionalParams: unknown[]): void {
    console.warn('The method "Logger.prototype.trace()" not implemented.');
  }
  get Console(): console.ConsoleConstructor {
    console.warn('ConsoleConstructor is not allowed to access by user scripts.');
    return null;
  }
  profile(label?: string): void {
    console.warn('The method "Logger.prototype.profile()" not implemented.');
  }
  profileEnd(label?: string): void {
    console.warn('The method "Logger.prototype.profileEnd()" not implemented.');
  }
}

const defaultLogger = new Logger();
/**
 * See https://nodejs.org/api/events.html#eventtarget-error-handling
 *
 * Becauset Node.js EventTarget will cause an uncaught exception on `process.nextTick()`, thus we
 * have no way to handle the error in better way, so we just log the error and not exiting the process.
 */
function handleGlobalExceptionOrRejection(err) {
  defaultLogger.warn(`
==============================
uncaught exception or rejection
Message: ${err?.message || err || 'null'}
Stack: ${err?.stack || 'null'}
==============================
  `);
  process.exit(1);
}
process.on('uncaughtException', handleGlobalExceptionOrRejection);
process.on('unhandledRejection', handleGlobalExceptionOrRejection);

// Just log the environment variables.
writeLogToNative(0, `created default logger successfully, and the following are environment variables:`);
for (const [key, value] of Object.entries(process.env)) {
  writeLogToNative(0, `  ${key}: ${value}`);
}

export const log = defaultLogger.log.bind(defaultLogger);
export const info = defaultLogger.info.bind(defaultLogger);
export const debug = defaultLogger.debug.bind(defaultLogger);
export const warn = defaultLogger.warn.bind(defaultLogger);
export const error = defaultLogger.error.bind(defaultLogger);

export function getLogger() {
  return defaultLogger;
}
