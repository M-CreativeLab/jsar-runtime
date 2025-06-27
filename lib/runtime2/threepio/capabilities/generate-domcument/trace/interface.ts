import { TimePoint } from "./TimePoint";

export type AllTraceTypes = TraceGenerateDomType;

/**
 * This type is used to categorize different tracing operations within the task flow.
 * @description
 * It can be one of the following:
 * 'fullFlow' for the entire task flow,
 * 'planRequest' for plan requests,
 * 'moudleRequest' for module requests,
 * 'DOMOperation' for DOM operations.
 */
export type TraceGenerateDomType = 'fullFlow' | 'planRequest' | 'moudleRequest' | 'DOMOperation';

/**
 * This type is used to categorize different tracing operations within the task flow.
 * @description
 * TraceGenerateDomType is a subset of TraceType. 
 * */
export type TraceType = string;

/**
 * define the trace configuration
 */
export type TraceConfiguration = {
  traceType: AllTraceTypes;
  data?: { [key: string]: any }
} & TraceOptions;

/**
 * define the trace options
 */
export type TraceOptions = {
  requestId: string;
  parentRequestId?: string;
};

export interface ISerializable {
  toJSON(): { [key: string]: any };
}

/**
 * Attributes is a map from string to attribute values.
 */
export interface Attributes {
  [attributeKey: string]: any;
}

export interface SpanAttributes {
  [key: string]: any;
}

/**
 * Span is an interface that represents a single span in the task flow.
 * The relationship between span and timepoint is like the relationship between a line and points.
 *  A line consists of multiple points (including endpoints start, end and other points)
 */
export interface Span extends ISerializable {
  readonly startPoint: TimePoint;
  readonly endPoint: TimePoint;
  readonly attributes: Attributes;
  readonly errorInfo: Error[];
  readonly traceType: TraceType;

  reportError(err: Error): void;
  setAttribute(key: string, value: any): this;
  setAttributes(attributes: SpanAttributes): this;
  end(): void;
}
