import { reportThreepioWarning } from "../utils/threepioLog";
import { TraceOptions, TraceType } from "./decorator";

export type SpanContext = TraceOptions & {
  name: string,
  traceType: TraceType,
  metadata?: Record<string, any>
};

export class TaskFlowSpan {
  public metrics: Record<string, any> = {};
  public errorInfo: Error | null = null;
  public children: TaskFlowSpan[] = [];
  public endTime: number | null = null;

  constructor(public context: SpanContext, public startTime = Date.now()) { }

  metric(key: string, value: any) {
    this.metrics[key] = value;
  }

  error(err: Error) {
    this.errorInfo = err;
  }

  end() {
    this.endTime = Date.now();
    if (this.errorInfo) {
      reportThreepioWarning(`[Trace] Error in ${this.context.traceType}:`, this.errorInfo);
    }
  }
}
