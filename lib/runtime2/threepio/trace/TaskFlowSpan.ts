import { reportThreepioWarning } from '../utils/threepioLog';
import { Span, TraceType } from './interface';
import { TraceOptions } from './wrapTaskFlowMonitor';

/**
 * This type is used to define the context of a span in the task flow.
 * It includes the name of the span, the type of trace, and optional metadata.
 * @description
 * SpanContext is used to provide additional information about the span,
 * such as its name, type, and any metadata that may be relevant.
 * It is used to create a new span in the task flow.
 */
export type SpanContext = TraceOptions & {
  name: string,
  traceType: TraceType,
  metadata?: Record<string, any>
};

export class TaskFlowSpan implements Span {
  public metrics: Record<string, any> = {};
  public errorInfo: Error | null = null;
  public timePointIds: string[] = [];
  public endTime: number = 0;
  public startTime: number = 0;

  constructor(public context: SpanContext) {
    this.start();
  }

  public metric(key: string, value: any) {
    this.metrics[key] = value;
  }

  public error(err: Error) {
    this.errorInfo = err;
  }

  public start() {
    this.startTime = performance.now()
  }

  public end(): void {
    this.endTime = performance.now();
    if (this.errorInfo) {
      reportThreepioWarning(`[Trace] Error in ${this.context.traceType}:`, this.errorInfo);
    }
  }

  public addTimePointId(timePointId: string) {
    this.timePointIds.push(timePointId);
  }

  toJSON(): { [key: string]: any } {
    return {
      traceType: this.context.traceType,
      name: this.context.name,
      startTime: this.startTime,
      endTime: this.endTime,
      timePointIds: this.timePointIds,
      metrics: this.metrics,
      metadata: this.context.metadata,
      errorInfo: this.errorInfo,
    }
  }
}
