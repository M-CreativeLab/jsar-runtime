import { EventEmitter } from 'events';
import {
  EmitData,
  FragmentType,
  MoudleFragment,
  ParsedHeader,
  ParsedModule,
  MoudleParserEventType,
  MoudleFragmentTask,
  HeaderFragment,
} from './interfaces';
import { getPlanPrompt, getWorkerPrompt } from './prompts';
import { callLLM } from '../../utils/llmClient';
import { ApiStream } from '../../api/transform/stream';
import { StreamPlannerParser } from './parsers/StreamPlannerParser';
import { reportThreepioError, reportThreepioInfo } from '../../utils/threepioLog';
import { TraceOptions, wrapTaskFlowMonitor } from '../../trace/wrapTaskFlowMonitor';
import { StreamHtmlParser } from './parsers/StreamHtmlParser';
import { HTMLStream, PlannerStream } from './parsers/interface';

type ProcessPlannerParam = {
  input: string,
  plannerParser: StreamPlannerParser;
  taskPromises: Promise<void>[];
} & TraceOptions;

/**
 * Converts a single parsed module into a SkeletonNode and an FragmentTask.
 * This is designed to be called incrementally as modules are parsed.
 * @param module Parsed module information.
 * @param overallDesignTheme The overall design theme from the page header.
 * @param appName The application name from the page header.
 * @returns An object containing the new SkeletonNode and its corresponding FragmentTask.
 */
function createModuleTask(parentRequestId: string, module: ParsedModule): MoudleFragmentTask {
  const systemPrompt = createPrompt({ pageGoal: module.description, parentId: parentRequestId });
  return {
    systemPrompt,
    input: JSON.stringify(module),
    parentRequestId,
    moudle: module,
    fragmentType: FragmentType.HTML,
    context: {
      pageGoal: module.name,
    },
  };
}

/**
 * Creates a prompt for generating HTML fragments based on the provided task.
 * The prompt includes the page goal, parent ID, and design system information.
 * @param task - The MoudleFragmentTask containing context and module information.
 * @returns An object containing the input JSON string and the formatted prompt string.
 */
export function createPrompt(options: { pageGoal: string, parentId: string, designSystemInfo?: string }): string {
  const { pageGoal, parentId, designSystemInfo } = options;
  let prompt = getWorkerPrompt();
  prompt = prompt.replace(/{{PAGE_GOAL}}/g, pageGoal);
  prompt = prompt.replace(/{{PARENT_ID}}/g, parentId);
  prompt = prompt.replace(/{{DESIGN_SYSTEM_INFO}}/g, designSystemInfo);
  return prompt;
}

export interface RequestFlowManager {
  on(event: MoudleParserEventType, listener: (data: EmitData) => void): this;
}

export class RequestFlowManager extends EventEmitter {
  public async executeFlow(input: string): Promise<void> {
    let taskPromises: Promise<void>[] = [];
    try {
      const planRequest = wrapTaskFlowMonitor(this.#processPlannerRequest.bind(this), { type: 'planRequest' });
      await planRequest(
        {
          input,
          taskPromises,
          parentRequestId: input,
          requestId: input,
          plannerParser: new StreamPlannerParser()
        }
        , { type: 'planRequest' });
      await Promise.all(taskPromises);
      reportThreepioInfo('All tasks completed.');
    } catch (error) {
      reportThreepioError('Error during flow execution:', error);
      throw error;
    }
  }

  async #processPlannerRequest(plannerParam: ProcessPlannerParam): Promise<void> {
    const systemPrompt = getPlanPrompt();
    const { input } = plannerParam;
    const { stream, requestId } = callLLM({ input, systemPrompt });
    const plannerItemStream = this.#processApiStream(stream);
    await this.#processPlannerStream({ requestId, parentRequestId: input, ...plannerParam }, plannerItemStream);
  }

  async* #processApiStream(stream: ApiStream): PlannerStream {
    const plannerParser = new StreamPlannerParser();
    const parsingPromise = (async () => {
      try {
        for await (const chunk of stream) {
          plannerParser.parseChunk(chunk);
        }
      } finally {
        plannerParser.endStream();
      }
    })();
    yield* plannerParser.stream();
    await parsingPromise;
  }

  async #processPlannerStream(plannerParam: ProcessPlannerParam, planStream: PlannerStream) {
    const { taskPromises, requestId, parentRequestId } = plannerParam;
    let headerHasBeenParsed = false;
    for await (const item of planStream) {
      switch (item.type) {
        case 'header':
          const header = item.data as ParsedHeader;
          headerHasBeenParsed = true;
          const layout = header.layout.replace(/height/g, 'min-height');
          reportThreepioInfo(`Header parsed layout: ${layout}`);
          this.#emitData('append', {
            requestId,
            parentRequestId,
            type: FragmentType.Header,
            fragment: { content: layout, rawContent: header } as HeaderFragment
          });
          break;
        case 'module':
          const module = item.data as ParsedModule;
          if (!headerHasBeenParsed) {
            reportThreepioError('Module parsed before header was processed. Aborting.');
            break;
          }
          const moduleParentId = 'moudle' + taskPromises.length;
          module.parentId = moduleParentId;
          const task = createModuleTask(requestId, module);
          this.#emitData('append', {
            type: FragmentType.Moudle,
            fragment: { id: moduleParentId, content: module.layout, rawContent: module } as MoudleFragment,
            requestId,
            parentRequestId
          });
          taskPromises.push(this.#createModuleFragments(task));
          break;
        case 'error':
          const errorData = item.data as { error: Error; content: string };
          reportThreepioError('Planner parsing error:', errorData.error.message);
          break;
        case 'end':
          const endData = item.data as { processedCount: number };
          reportThreepioInfo('Planner parsing completed. Processed:', endData.processedCount);
          break;
      }
    }
  }

  async #createModuleFragments(task: MoudleFragmentTask): Promise<void> {
    try {
      for await (const fragment of this.#createMoudleStream(task)) {
        if (fragment.eventType === 'append') {
          this.#emitData('append', fragment.data);
        } else if (fragment.error) {
          reportThreepioError('Fragment generation error:', fragment.error);
        }
      }
    } catch (error) {
      reportThreepioError(`Error generating fragment for task ${task}:`, error);
    }
  }

  #createMoudleStream(task: MoudleFragmentTask): HTMLStream {
    const { systemPrompt, input } = task;
    const { stream, requestId } = callLLM({ input, systemPrompt });
    task.requestId = requestId;
    const wrap = wrapTaskFlowMonitor(this.#processMoudleStream.bind(this), { type: 'moudleRequest' });
    return wrap(task, stream);
  }

  async * #processMoudleStream(task: MoudleFragmentTask, stream: ApiStream): HTMLStream {
    const htmlParser = new StreamHtmlParser(task.parentRequestId);
    const parserStreamPromise = (async function* () {
      for await (const item of htmlParser.stream()) {
        if (item.eventType === 'append') {
          item.data.requestId = task.requestId;
          item.data.parentRequestId = task.parentRequestId;
        }
        yield item;
      }
    })();

    const inputPromise = (async () => {
      for await (const chunk of stream) {
        htmlParser.parseChunk(chunk);
      }
      htmlParser.endStream();
    })();

    yield* parserStreamPromise;
    await inputPromise;
  }

  #emitData(event: string, data: EmitData) {
    this.emit(event, data);
  }
}
