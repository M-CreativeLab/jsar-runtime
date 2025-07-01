import { Span, SpanOptions } from './Span';
import { TraceManager } from './TraceManager';

export const traceManager = new TraceManager();

export function startSpan<F extends (span: Span) => unknown>(options: SpanOptions, fn: F): ReturnType<F> {
  const span = traceManager.startSpan(options);
  traceManager.buildCallGraph(options.context.requestId, options.context.parentRequestId);
  try {
    const result = fn(span);
    if (result instanceof Promise) {
      return result
        .catch((error) => { span.reportError(error as Error); })
        .finally(() => span.end()) as ReturnType<F>;
    }
    span.end();
    return result as ReturnType<F>;
  } catch (error) {
    span.reportError(error as Error);
    span.end();
    throw error;
  }
}
