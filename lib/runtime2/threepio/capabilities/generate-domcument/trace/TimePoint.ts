import { ISerializable, TraceType } from './interface';

export type TimePointOptions = {
  type: TraceType,
  requestId: string,
};

export class TimePoint implements ISerializable {
  type: TraceType;
  time: number = performance.now();
  id: number;
  referenceId: string;
  private static Counter: number = 0;

  constructor(options: TimePointOptions) {
    this.type = options.type;
    this.referenceId = options.requestId;
    this.id = TimePoint.Counter++;
  }

  toJSON(): { [key: string]: any; } {
    return {
      type: this.type,
      time: this.time,
      id: this.id,
      referenceId: this.referenceId,
    };
  }
}
