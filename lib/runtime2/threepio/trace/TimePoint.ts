import { ISerializable, TraceType } from './interface';

export type TimePointOptions = {
  type: TraceType,
  time: number,
  requestId: string,
};

export class TimePoint implements ISerializable {
  type: TraceType;
  time: number;
  id: string;
  referenceId: string;
  private static Counter: number = 0;
  constructor(options: TimePointOptions) {
    this.type = options.type;
    this.time = options.time;
    this.referenceId = options.requestId;
    this.id = (TimePoint.Counter++).toString();
  }

  toJSON(): { [key: string]: any; } {
    return {
      type: this.type,
      time: this.time,
      id: this.id,
      referenceId: this.referenceId,
    }
  }
}
