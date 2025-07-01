import { TimePoint } from './TimePoint';
import { TraceManager } from './TraceManager';
import { reportThreepioWarning } from '../../../utils/threepioLog';
import { Attributes, ISerializable, SpanAttributes, TraceOptions, TraceType } from './interface';

/**
 * SpanContext is used to provide additional information about the span,
 * such as its name, type, and any metadata that may be relevant.
 * It is used to create a new span in the task flow.
 */
export type SpanContext = {
  manager?: TraceManager,
} & TraceOptions;

/**
 * SpanOptions is used to define the options for creating a new span in the  flow.
 * It includes the name of the span, the type of trace, and the context of the span.
 */
export type SpanOptions = {
  name: string,
  traceType: TraceType,
  context: SpanContext,
};

export class Span implements ISerializable {
  readonly attributes: Attributes = {};
  readonly context: SpanContext;
  readonly traceType: TraceType;
  readonly errorInfo: Error[] = [];
  public readonly startPoint: TimePoint;
  public readonly endPoint: TimePoint;

  constructor(options: SpanOptions) {
    this.context = options.context;
    this.traceType = options.traceType;
    this.startPoint = this.#createTimePoint();
    this.endPoint = this.#createTimePoint();
  }

  getSpanContext(): SpanContext {
    return this.context;
  }

  setAttribute(key: string, value: any): this {
    this.attributes[key] = value;
    return this;
  }

  setAttributes(attributes: SpanAttributes): this {
    Object.assign(this.attributes, attributes);
    return this;
  }

  reportError(err: Error): void {
    this.errorInfo.push(err);
  }

  end(): void {
    this.endPoint.time = performance.now();
    if (this.errorInfo.length > 0) {
      reportThreepioWarning(`[Trace] Error in ${this.traceType}:`, this.errorInfo);
    }
  }

  toJSON(): { [key: string]: any } {
    return {
      traceType: this.traceType,
      startTime: this.startPoint,
      endTime: this.endPoint,
      errorInfo: this.errorInfo,
      attributes: this.attributes,
    };
  }

  #createTimePoint(): TimePoint {
    const timePoint = new TimePoint({
      type: this.traceType,
      requestId: this.context.requestId,
    });
    if (this.context.manager) {
      this.context.manager.addTimePoint(timePoint);
    }
    return timePoint;
  }
}
