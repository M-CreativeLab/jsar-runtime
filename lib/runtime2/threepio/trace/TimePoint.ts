import { v4 as uuidv4 } from 'uuid';
import { TraceType } from "./interface";

/**
 * TimePointStatus
 * start: the time point is the start of the span
 * end: the time point is the end of the span
 * intermediate: the time point is the intermediate of the span
 */
export type TimePointStatus = 'start' | 'end' | 'intermediate';

export type TimePointOptions = {
  type: TraceType,
  time: number,
  requestId: string,
  status: TimePointStatus
}

export class TimePoint {
  type: TraceType;
  time: number;
  id: string;
  callId: string;
  status: TimePointStatus;

  constructor(options: TimePointOptions) {
    this.type = options.type;
    this.time = options.time;
    this.status = options.status;
    this.callId = options.requestId;
    this.id = uuidv4();
  }
}