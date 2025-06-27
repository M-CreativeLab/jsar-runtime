import util from 'util';
import { Span } from './interface';
import { TimePoint } from './TimePoint';
import { SpanImp, SpanOptions } from './Span';

interface CallNode {
  requestId: string;
  children: CallNode[];
}

export class TraceManager {
  #spans: Span[] = [];
  #callGraghs: CallNode[] = [];
  #timePoints: TimePoint[] = [];

  startSpan(options: SpanOptions): Span {
    const { requestId, parentRequestId } = options.context;
    options.context.timepointsRef = this.#timePoints;
    const span = new SpanImp(options) as Span;
    this.#spans.push(span);
    // build call graph
    this.#getOrNewCallNode(requestId, parentRequestId);
    return span;
  }

  public addError(span: Span, err: Error): void {
    span.reportError(err);
  }

  public endSpan(span: Span) {
    span.end();
  }

  public printAll(): void {
    console.log(
      util.inspect(this.#callGraghs.map((root) => this.toJSON(root)),
      ), { depth: null, colors: true },
      util.inspect(this.#spans.map(span => span.toJSON()),
        { depth: null, colors: true }),
      util.inspect(this.#timePoints,
        { depth: null, colors: true }));
  }

  public print() {
    console.log(this.#callGraghs.map((root) => this.toJSON(root)));
    console.log(this.#spans);
    console.log(this.#timePoints);
  }

  public toJSON(node: CallNode) {
    return {
      requestId: node.requestId,
      children: node.children.map(child => this.toJSON(child)),
    }
  }

  #getOrNewCallNode(requestId: string, parentRequestId: string): CallNode {
    let node = this.#findCallNodeByRequestId(requestId);
    if (requestId == null) {
      throw new Error('requestId is required');
    }
    if (!node) {
      node = {
        requestId,
        children: [],
      };
      if (parentRequestId) {
        const parent = this.#findCallNodeByRequestId(parentRequestId);
        parent.children.push(node);
      } else {
        this.#callGraghs.push(node);
      }
    }
    return node;
  }

  #findCallNodeByRequestId(requestId: string, callNodes: CallNode[] = this.#callGraghs): CallNode | undefined {
    for (const node of callNodes) {
      if (node.requestId === requestId) return node;
      if (node.children.length > 0) {
        const found = this.#findCallNodeByRequestId(requestId, node.children);
        if (found) return found;
      }
    }
    return undefined;
  }
}
