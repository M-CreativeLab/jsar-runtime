import { Span, SpanOptions } from './Span';
import { TraceManager } from './TraceManager';

export const traceManager = new TraceManager();

export function startSpan<F extends (span: Span) => unknown>(options: SpanOptions, fn: F): ReturnType<F> {
  const span = traceManager.startSpan(options);
  traceManager.buildCallGraph(options.context.requestId, options.context.parentRequestId);
  try {
    const result = fn(span);
    if (result && typeof result === 'object' &&
      typeof (result as any)[Symbol.asyncIterator] === 'function') {
      return wrapAsyncGenerator(result as AsyncGenerator<any, any, any>, span) as ReturnType<F>;
    }
    if (result instanceof Promise) {
      return result.finally(() => span.end()) as ReturnType<F>;
    }
    span.end();
    return result as ReturnType<F>;
  } catch (error) {
    span.reportError(error as Error);
    span.end();
    throw error;
  }
}

export function startGeneratorSpan<T>(
  options: SpanOptions,
  generatorFn: (span: Span) => AsyncGenerator<T>
): AsyncGenerator<T> {
  return (async function* () {
    const span = traceManager.startSpan(options);
    try {
      const generator = generatorFn(span);
      for await (const value of generator) {
        yield value;
      }
    } catch (error) {
      span.reportError(error as Error);
      throw error;
    } finally {
      span.end();
    }
  })();
}

function wrapAsyncGenerator(
  generator: AsyncGenerator<any>,
  span: Span
): AsyncGenerator<any> {
  async function* wrapped() {
    for await (const value of generator) {
      yield value;
    }
    traceManager.endSpan(span);
  }
  return wrapped();
}
