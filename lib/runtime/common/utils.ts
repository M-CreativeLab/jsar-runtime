import * as BABYLON from 'babylonjs';
import * as logger from '../../bindings/logger';
import { getPerformanceInCurrentEnv } from './performance';

const performance = getPerformanceInCurrentEnv();

/**
 * Get the vertex data from a BABYLON.VertexBuffer object.
 */
export function toVertexData(buffer: BABYLON.VertexBuffer) {
  if (!buffer) {
    return null;
  }
  const data = buffer.getData();
  if (!data) {
    return null;
  }
  const isArrayBufferOrView = data instanceof ArrayBuffer || ArrayBuffer.isView(data);
  if (isArrayBufferOrView) {
    if (data instanceof ArrayBuffer) {
      return new Float32Array(data);
    } else if (data instanceof Float32Array) {
      return data;
    } else if (data instanceof Uint8Array) {
      return new Float32Array(data.buffer, data.byteOffset, data.byteLength / Float32Array.BYTES_PER_ELEMENT);
    }
  } else if (Array.isArray(data)) {
    return new Float32Array(data);
  }
  throw new TypeError('Unknown data type: ' + data.constructor.name);
}

/**
 * To indices array.
 * @param {BABYLON.IndicesArray} data 
 * @returns
 */
export function toIndicesArray(data: BABYLON.IndicesArray, reversed = false): Uint32Array {
  if (!data) {
    return null;
  }

  if (data instanceof Uint32Array) {
    if (reversed === true) {
      data.reverse();
    }
    return data;
  }

  if (ArrayBuffer.isView(data)) {
    const dataArray = Array.from(data);
    if (reversed === true) {
      dataArray.reverse();
    }
    return new Uint32Array(dataArray);
  }

  if (Array.isArray(data)) {
    if (reversed === true) {
      data.reverse();
    }
    return new Uint32Array(data);
  }
}

export async function executeWithTimeProfiler<T>(tag: string, fn: (...args: any[]) => Promise<T> | T): Promise<T> {
  const startTime = performance.now();

  let r: T;
  const valueFuture = fn();
  if (valueFuture instanceof Promise) {
    r = await valueFuture;
  } else {
    r = valueFuture;
  }
  const endTime = performance.now();
  const executionTime = endTime - startTime;
  const message = `${tag} takes ${Math.floor(executionTime)}ms`;

  logger.info(message);
  return r;
}
