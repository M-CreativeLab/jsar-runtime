/**
 * This type is used to categorize different tracing operations within the task flow.
 * @description
 * TraceGenerateDomType is a subset of TraceType. 
 * */
export type TraceType = TraceGenerateDomType;

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
 * Span is an interface that represents a single span in the task flow.
 * It contains  metrics, error information, and start and end time.
 */
export interface Span {
  metric(key: string, value: any);
  error(err: Error);
  start(): void;
  end(): void;
  toJSON(): { [key: string]: any };
}
