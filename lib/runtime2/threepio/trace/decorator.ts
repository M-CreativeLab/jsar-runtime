import { TaskFlowSpan, SpanContext } from './TaskFlowSpan';
import { TraceManager } from './TraceManager';
import { reportThreepioError } from '../utils/threepioLog';
import { EmitData, MoudleFragmentTask } from '../capabilities/generate-domcument/interfaces';

export type TraceType = 'task' | 'planRequest' | 'moudleRequest' | 'opera';

export type TraceOptions = {
  requestId?: string;
  parentRequestId?: string;
}

interface TraceConfiguration {
  type: TraceType;
}

export function MonitorTaskFlow(config: TraceConfiguration) {
  return function (_target: any, _propertyKey: string, descriptor: PropertyDescriptor) {
    const originalMethod = descriptor.value;
    descriptor.value = function (...args: any[]) {
      const ctx = createSpanContext(config, args);
      const span = TraceManager.getSpan(ctx);
      const result = originalMethod.apply(this, args);
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
            span.end();
          });
      }
    };
  };
}

function wrapAsyncGenerator(
  gen: AsyncGenerator<any>,
  span: TaskFlowSpan
): AsyncGenerator<any> {
  async function* wrapped() {
    for await (const value of gen) {
      yield value;
    }
    span.end();
  }
  return wrapped();
}

function createSpanContext(config: TraceConfiguration, args: any[]): SpanContext {
  const traceType = config.type;
  const ctx = { traceType } as SpanContext;
  switch (traceType) {
    case 'task':
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
    case 'opera':
      {
        const data = args[0] as EmitData;
        let fieldName = `opera`;//-${data.type.toString()}`;
        const metadata = {
          [fieldName]: {
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
