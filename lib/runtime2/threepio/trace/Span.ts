import { reportThreepioWarning } from '../utils/threepioLog';
import { AllTraceTypes, Attributes, Span, SpanAttributes, TraceOptions } from './interface';
import { TimePoint } from './TimePoint';

type TraceState = 'ok' | 'error';
/**
 * SpanContext is used to provide additional information about the span,
 * such as its name, type, and any metadata that may be relevant.
 * It is used to create a new span in the task flow.
 */
export type SpanContext = {
  timepointsRef?: TimePoint[],
  traceId?: string,
  spanId?: string
  traceState?: TraceState,
} & TraceOptions;

/**
 * SpanOptions is used to define the options for creating a new span in the  flow.
 * It includes the name of the span, the type of trace, and the context of the span.
 */
export type SpanOptions = {
  name: string,
  traceType: AllTraceTypes,
  context: SpanContext,
};

export class SpanImp implements Span {
  readonly attributes: Attributes = {};
  readonly context: SpanContext;
  readonly traceType: AllTraceTypes;
  readonly errorInfo: Error[] = [];
  readonly startPoint: TimePoint;
  public endPoint: TimePoint;

  constructor(options: SpanOptions) {
    this.context = options.context;
    this.traceType = options.traceType;
    this.startPoint = this.#createTimePoint()
  }

  public getSpanContext(): SpanContext {
    return this.context;
  }

  public setAttribute(key: string, value: any): this {
    this.attributes[key] = value;
    return this;
  }

  public setAttributes(attributes: SpanAttributes): this {
    Object.assign(this.attributes, attributes);
    return this;
  }

  public reportError(err: Error): void {
    this.context.traceState = 'error';
    this.errorInfo.push(err);
  }

  public end(): void {
    this.endPoint = this.#createTimePoint();
    if (this.errorInfo.length > 0) {
      reportThreepioWarning(`[Trace] Error in ${this.traceType}:`, this.errorInfo);
    }
  }

  public toJSON(): { [key: string]: any } {
    return {
      traceType: this.traceType,
      startTime: this.startPoint,
      endTime: this.endPoint,
      errorInfo: this.errorInfo,
    }
  }

  #createTimePoint(): TimePoint {
    const timePoint = new TimePoint({
      type: this.traceType,
      time: performance.now(),
      requestId: this.context.requestId,
    });
    if (this.context.timepointsRef) {
      this.context.timepointsRef.push(timePoint);
    }
    return timePoint;
  }
}
