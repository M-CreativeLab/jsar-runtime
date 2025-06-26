import { TraceType } from './interface';
import { TraceManager } from './TraceManager';
import { reportThreepioError } from '../utils/threepioLog';
import { TaskFlowSpan, SpanContext } from './TaskFlowSpan';
import { EmitData, MoudleFragmentTask } from '../capabilities/generate-domcument/interfaces';

export const traceManager = new TraceManager();

/**
 * define the trace options
 */
export type TraceOptions = {
  requestId?: string;
  parentRequestId?: string;
};

/**
 * define the trace configuration
 */
interface TraceConfiguration {
  type: TraceType;
}

/**
 * Wraps a function with a task flow monitor.
 * how to use:
 * wrapTaskFlowMonitor(async (input) => {await manager.executePlan(input);}, { type: 'planRequest' })(input);
 * Determine the span type based on the type parameter to handle different tracing logic
 * @param fn The function to be traced
 * @param config The configuration for the trace
 * @returns 
 */
export function wrapTaskFlowMonitor<T extends (...args: any[]) => any>(fn: T, config: TraceConfiguration): T {
  return (function (this: any, ...args: any[]) {
    const ctx = createSpanContext(config, args);
    const span = traceManager.createSpan(ctx);
    const result = fn.apply(this, args);
    if (result && typeof result[Symbol.asyncIterator] === 'function') {
      return wrapAsyncGenerator(result, span);
    } else {
      return Promise.resolve(result)
        .then(async (res) => {
          return res;
        })
        .catch((err) => {
          span.error(err);
          throw err;
        })
        .finally(() => {
          traceManager.endSpan(span);
        });
    }
  }) as T;
}

/**
 * Creates a span context for a given configuration and arguments.
 * 
 * @param config The configuration for the trace
 * @param args The arguments passed to the function
 * @returns 
 */
function createSpanContext(config: TraceConfiguration, args: any[]): SpanContext {
  const traceType = config.type;
  const ctx = { traceType } as SpanContext;
  switch (traceType) {
    case 'fullFlow':
      return {
        requestId: args[0],
        parentRequestId: '',
        metadata: {},
        ...ctx
      };
    case 'planRequest':
      {
        const planParam = args[0] as TraceOptions;
        return {
          requestId: planParam.requestId,
          parentRequestId: planParam.parentRequestId,
          metadata: {},
          ...ctx
        };
      }
    case 'DOMOperation':
      {
        const data = args[0] as EmitData;
        const metadata = {
          'DOMOperation': {
            type: data.type,
            data: data.fragment,
            startTime: Date.now(),
          }
        }
        return {
          requestId: data.requestId,
          parentRequestId: data.parentRequestId,
          metadata,
          ...ctx
        };
      }
    case 'moudleRequest':
      {
        const data = args[0] as MoudleFragmentTask;
        return {
          traceType,
          name: data.context.pageGoal,
          requestId: data.requestId,
          parentRequestId: data.parentRequestId,
          metadata: {
            input: data.input,
            systemPrompt: data.systemPrompt,
          },
          ...ctx
        };
      }
    default:
      reportThreepioError(`Unknown traceable type: ${name}`);
      break;
  }
}

function wrapAsyncGenerator(
  generator: AsyncGenerator<any>,
  span: TaskFlowSpan
): AsyncGenerator<any> {
  async function* wrapped() {
    for await (const value of generator) {
      yield value;
    }
    traceManager.endSpan(span);
  }
  return wrapped();
}
