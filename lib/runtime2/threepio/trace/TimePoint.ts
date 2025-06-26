import { TraceType } from './interface';

export type TimePointOptions = {
  type: TraceType,
  time: number,
  requestId: string,
};

export class TimePoint {
  type: TraceType;
  time: number;
  id: string;
  referenceId: string;
  static counter: number = 0;
  constructor(options: TimePointOptions) {
    this.type = options.type;
    this.time = options.time;
    this.referenceId = options.requestId;
    this.id = (TimePoint.counter++).toString();
  }
}
