import util from 'util';
import fs from 'fs';
import { TimePoint } from './TimePoint';
import { Span, SpanOptions } from './Span';
interface CallNode {
  requestId: string;
  children: CallNode[];
}

export class TraceManager {
  #spans: Span[] = [];
  #callGraghs: CallNode[] = [];
  #timePoints: TimePoint[] = [];

  startSpan(options: SpanOptions): Span {
    options.context.manager = this;
    const span = new Span(options) as Span;
    this.#spans.push(span);
    return span;
  }

  buildCallGraph(requestId: string, parentRequestId: string): void {
    this.#getOrNewCallNode(requestId, parentRequestId);
  }

  addTimePoint(timePoint: TimePoint): void {
    this.#timePoints.push(timePoint);
  }

  addError(span: Span, err: Error): void {
    span.reportError(err);
  }

  endSpan(span: Span) {
    span.end();
  }

  printAll(): void {
    console.log(
      util.inspect(this.#callGraghs.map((root) => this.toJSON(root)),
      ), { depth: null, colors: true },
      util.inspect(this.#spans.map(span => span.toJSON()),
        { depth: null, colors: true }),
      util.inspect(this.#timePoints,
        { depth: null, colors: true }));
  }

  print() {
    console.log(this.#callGraghs.map((root) => this.toJSON(root)));
    console.log(this.#spans);
    console.log(this.#timePoints);
  }

  toJSON(node: CallNode) {
    return {
      requestId: node.requestId,
      children: node.children.map(child => this.toJSON(child)),
    };
  }

  save(input: string, html: string, dir: string): void {
    const callGrahgs = this.#callGraghs.map((root) => this.toJSON(root));
    const spans = this.#spans.map(span => span.toJSON());
    const timePoints = this.#timePoints;
    const jsonData = {
      callGrahgs: callGrahgs,
      spans: spans,
      timePoints: timePoints,
      html: html,
    }
    dir = process.cwd() + dir;
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    const name = input + new Date().toISOString().replace(/[:.]/g, '-');
    const path = `${dir}/${name}.trace.json`;
    fs.promises.writeFile(path, JSON.stringify(jsonData, null, 2))
      .then(() => {
        console.log(`Trace saved to ${path}`);
      })
      .catch((err) => {
        console.error(`Error saving trace data: ${err}`);
      });
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
