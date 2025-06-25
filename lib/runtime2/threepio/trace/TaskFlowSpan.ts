import { reportThreepioWarning } from '../utils/threepioLog';
import { TraceType } from './interface';
import { TraceOptions } from './wrapTaskFlowMonitor';

export type SpanContext = TraceOptions & {
  name: string,
  traceType: TraceType,
  metadata?: Record<string, any>
};

export class TaskFlowSpan {
  public metrics: Record<string, any> = {};
  public errorInfo: Error | null = null;
  public children: TaskFlowSpan[] = [];
  public timePointIds: string[] = [];

  constructor(public context: SpanContext) { }

  public metric(key: string, value: any) {
    this.metrics[key] = value;
  }

  public error(err: Error) {
    this.errorInfo = err;
  }

  public end(timePointId: string) {
    this.addTimePointId(timePointId);
    if (this.errorInfo) {
      reportThreepioWarning(`[Trace] Error in ${this.context.traceType}:`, this.errorInfo);
    }
  }

  public addTimePointId(timePointId: string) {
    this.timePointIds.push(timePointId);
  }
}
