import util from 'util';
import { TimePoint, TimePointOptions } from "./TimePoint";
import { TaskFlowSpan, SpanContext } from "./TaskFlowSpan";

export class TraceManager {
  public static Instance = new TraceManager();
  #callGragh: TaskFlowSpan[] = [];
  #timePoints: TimePoint[] = [];

  public getSpan(context: SpanContext): TaskFlowSpan {
    let span = this.#findSpanByRequestId(context.requestId);
    // insert time point
    const timePoint = this.#createTimePoint({
      type: context.traceType,
      requestId: context.requestId,
      status: 'start',
    });
    if (span) {
      if (context.metadata) {
        this.#updateMetadata(context.metadata, span);
      }
      timePoint.status = 'intermediate';
    } else {
      timePoint.status = 'start';
      span = this.#createSpan(context);
    }
    span.addTimePointId(timePoint.id);
    this.#addTimePoint(timePoint);
    return span;
  }

  public getTimePointsByCallId(callId: string): TimePoint[] {
    return this.#timePoints.filter(p => p.callId === callId);
  }

  public updateSpanMetric(requestId: string, key: string, value: number) {
    const span = this.#findSpanByRequestId(requestId);
    if (span) {
      span.metric(key, value);
    }
  }


  public endSpan(span: TaskFlowSpan) {
    const timePoint = this.#createTimePoint({
      type: span.context.traceType,
      requestId: span.context.requestId,
      status: 'end',
    });
    span.end(timePoint.id);
  }

  public addError(requestId: string, err: Error) {
    const span = this.#findSpanByRequestId(requestId);
    if (span) {
      span.error(err);
    }
  }

  public printAll(): void {
    console.log(util.inspect(this.#callGragh.map((root) => this.#spanToJSON(root)), { depth: null, colors: true }));
  }

  #updateMetadata(metadata: Record<string, any>, span: TaskFlowSpan) {
    Object.entries(metadata).forEach(([key, value]) => {
      span.context.metadata[key] = span.context.metadata[key]
        ? [...(Array.isArray(span.context.metadata[key]) ? span.context.metadata[key] : [span.context.metadata[key]]), value]
        : [value];
    });
  }

  #addTimePoint(timePoint: TimePoint) {
    this.#timePoints.push(timePoint);
  }

  #findSpanByRequestId(requestId: string, spans: TaskFlowSpan[] = this.#callGragh): TaskFlowSpan | undefined {
    for (const span of spans) {
      if (span.context.requestId === requestId) return span;
      if (span?.children.length > 0) {
        const found = this.#findSpanByRequestId(requestId, span.children);
        if (found) return found;
      }
    }
    return undefined;
  }

  #createTimePoint(timePoint: Omit<TimePointOptions, 'time'>) {
    const tp = new TimePoint({
      time: Date.now(),
      ...timePoint,
    });
    this.#addTimePoint(tp);
    return tp;
  }

  #createSpan(context: SpanContext): TaskFlowSpan {
    const span = new TaskFlowSpan(context);
    if (context.parentRequestId) {
      const parent = this.#findSpanByRequestId(context.parentRequestId);
      parent.children.push(span);
    } else {
      this.#callGragh.push(span);
    }
    return span;
  }

  #spanToJSON(span: TaskFlowSpan): any {
    return {
      traceType: span.context.traceType,
      name: span.context.name,
      requestId: span.context.requestId,
      parentRequestId: span.context.parentRequestId,
      metrics: span.metrics,
      timePoints: span.timePointIds,
      metadata: span.context.metadata,
      childCout: span.children.length,
      children: span.children.map((child) => this.#spanToJSON(child)),
      error: span.errorInfo?.message,
    };
  }
}
