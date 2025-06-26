import util from 'util';
import { TimePoint, TimePointOptions } from './TimePoint';
import { TaskFlowSpan, SpanContext } from './TaskFlowSpan';

interface CallNode {
  requestId: string;
  TaskFlowSpan: TaskFlowSpan[];
  children: CallNode[];
}
export class TraceManager {
  #callGragh: CallNode[] = [];
  #timePoints: TimePoint[] = [];

  public getOrNewSpan(context: SpanContext): TaskFlowSpan {
    let span = this.#findSpanByRequestId(context.requestId);
    if (span) {
      if (context.metadata) {
        this.#updateMetadata(context.metadata, span);
      }
      // insert time point
      const timePoint = this.#createTimePoint({
        type: context.traceType,
        requestId: context.requestId,
      });
      span.addTimePointId(timePoint.id);
      this.#addTimePoint(timePoint);
    } else {
      span = this.#createSpan(context);
    }
    return span;
  }

  public createSpan(ctx: SpanContext): TaskFlowSpan {
    const node = this.#getOrNewCallNode(ctx.requestId, ctx.parentRequestId);
    const span = this.#createSpan(ctx);
    const timePoint = this.#createTimePoint({
      type: ctx.traceType,
      requestId: ctx.requestId,
    });
    span.addTimePointId(timePoint.id);
    node.TaskFlowSpan.push(span);
    this.#addTimePoint(timePoint);
    return span;
  }

  public getTimePointsByCallId(callId: string): TimePoint[] {
    return this.#timePoints.filter(p => p.referenceId === callId);
  }

  public updateSpanMetric(requestId: string, key: string, value: number) {
    const span = this.#findSpanByRequestId(requestId);
    if (span) {
      span.metric(key, value);
    }
  }

  public addError(requestId: string, err: Error) {
    const span = this.#findSpanByRequestId(requestId);
    if (span) {
      span.error(err);
    }
  }

  public endSpan(span: TaskFlowSpan) {
    span.end();
  }

  public printAll(): void {
    console.log(util.inspect(this.#callGragh.map((root) => this.toJSON(root)
    ), { depth: null, colors: true }));
  }

  public print() {
    console.log(this.#callGragh.map((root) => this.toJSON(root)));
    console.log(this.#timePoints);
  }

  public toJSON(node: CallNode) {
    return {
      children: node.children.map(child => this.toJSON(child)),
      TaskFlowSpan: node.TaskFlowSpan.map(span => span.toJSON()),
    }
  }

  #getOrNewCallNode(requestId: string, parentRequestId: string): CallNode {
    let node = this.#findCallNodeByRequestId(requestId);
    if (!node) {
      node = {
        requestId,
        children: [],
        TaskFlowSpan: [],
      };
      if (parentRequestId) {
        const parent = this.#findCallNodeByRequestId(parentRequestId);
        parent.children.push(node);
      } else {
        this.#callGragh.push(node);
      }
    }
    return node;
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

  #findSpanByRequestId(requestId: string): TaskFlowSpan | undefined {
    for (const node of this.#callGragh) {
      for (const span of node.TaskFlowSpan) {
        if (span.context.requestId === requestId) {
          return span;
        }
      }
    }
    return undefined;
  }

  #findCallNodeByRequestId(requestId: string, callNodes: CallNode[] = this.#callGragh): CallNode | undefined {
    for (const node of callNodes) {
      if (node.requestId === requestId) return node;
      if (node?.children.length > 0) {
        const found = this.#findCallNodeByRequestId(requestId, node.children);
        if (found) return found;
      }
    }
    return undefined;
  }

  #createTimePoint(timePoint: Omit<TimePointOptions, 'time'>) {
    const tp = new TimePoint({
      time: performance.now(),
      ...timePoint,
    });
    return tp;
  }

  #createSpan(context: SpanContext): TaskFlowSpan {
    return new TaskFlowSpan(context);
  }
}
