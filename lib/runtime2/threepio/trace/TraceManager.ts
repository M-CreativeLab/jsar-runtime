import util from 'util';
import { TaskFlowSpan, SpanContext } from "./TaskFlowSpan";
import { reportThreepioWarning } from "../utils/threepioLog";

export class TraceManager {
  private static RootSpans: TaskFlowSpan[] = [];

  public static getSpan(context: SpanContext): TaskFlowSpan {
    const span = this.findSpanByRequestId(context.requestId);
    if (span) {
      this.updateMetadata(context);
      return span;
    } else {
      return this.createSpan(context);
    }
  }

  public static findSpanByRequestId(requestId: string, spans: TaskFlowSpan[] = this.RootSpans): TaskFlowSpan | undefined {
    for (const span of spans) {
      if (span.context.requestId === requestId) return span;
      if (span?.children.length > 0) {
        const found = this.findSpanByRequestId(requestId, span.children);
        if (found) return found;
      }
    }
    return undefined;
  }

  public static updateSpanMetric(requestId: string, key: string, value: number) {
    const span = this.findSpanByRequestId(requestId);
    if (span) {
      span.metric(key, value);
    }
  }

  public static updateMetadata(ctx: SpanContext) {
    const span = this.findSpanByRequestId(ctx.requestId);
    if (span) {
      Object.entries(ctx.metadata).forEach(([key, value]) => {
        span.context.metadata[key] = span.context.metadata[key]
          ? [...(Array.isArray(span.context.metadata[key]) ? span.context.metadata[key] : [span.context.metadata[key]]), value]
          : [value];
      });
    } else {
      reportThreepioWarning(`TaskFlowSpan not found for requestId: ${ctx.requestId}`);
    }
  }

  public static addError(requestId: string, err: Error) {
    const span = this.findSpanByRequestId(requestId);
    if (span) {
      span.error(err);
    }
  }

  public static endSpan(requestId: string) {
    const span = this.findSpanByRequestId(requestId);
    if (span) {
      span.end();
    }
  }

  public static clear() {
    this.RootSpans = [];
  }

  public static printAll(): void {
    console.log(util.inspect(this.RootSpans.map((root) => this.spanToJSON(root)), { depth: null, colors: true }));
  }

  private static createSpan(context: SpanContext): TaskFlowSpan {
    const span = new TaskFlowSpan(context, Date.now());
    if (context.parentRequestId) {
      const parent = this.findSpanByRequestId(context.parentRequestId);
      parent.children.push(span);
    } else {
      this.RootSpans.push(span);
    }
    return span;
  }

  private static spanToJSON(span: TaskFlowSpan): any {
    return {
      traceType: span.context.traceType,
      name: span.context.name,
      startTime: span.startTime,
      endTime: span.endTime,
      requestId: span.context.requestId,
      parentRequestId: span.context.parentRequestId,
      metrics: span.metrics,
      metadata: span.context.metadata,
      childCout: span.children.length,
      children: span.children.map((child) => this.spanToJSON(child)),
      error: span.errorInfo?.message,
    };
  }
}
