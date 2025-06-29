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
import { StreamHtmlParser } from './parsers/StreamHtmlParser';
import { TraceOptions } from './trace/interface';
import { startSpan, } from './trace/withFlowMonitoring';

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
    requestId: '',
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
      await this.#processPlannerRequest({
        input,
        taskPromises,
        parentRequestId: input,
        requestId: input,
        plannerParser: new StreamPlannerParser()
      });
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
    await startSpan({
      traceType: 'planRequest',
      name: input,
      context: {
        requestId,
        parentRequestId: input
      },
    }, async (span) => {
      span.setAttributes({ systemPrompt, ...plannerParam });
      await this.#processPlannerStream({ requestId, parentRequestId: input, ...plannerParam }, stream);
    })
  }

  async #processPlannerStream(plannerParam: ProcessPlannerParam, stream: ApiStream): Promise<void> {
    const plannerParser = new StreamPlannerParser();
    const inputPromise = (async () => {
      for await (const chunk of stream) {
        plannerParser.parseChunk(chunk);
      }
      plannerParser.endStream();
    })();
    const { taskPromises, requestId, parentRequestId } = plannerParam;
    let headerHasBeenParsed = false;
    for await (const item of plannerParser.stream()) {
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
          taskPromises.push(this.#createMoudleStream(task));
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
    await inputPromise;
  }

  async #createMoudleStream(task: MoudleFragmentTask): Promise<void> {
    const { systemPrompt, input } = task;
    const { stream, requestId } = callLLM({ input, systemPrompt });
    task.requestId = requestId;
    await startSpan({
      traceType: 'moudleRequest',
      name: task.context.pageGoal,
      context: {
        requestId: task.requestId,
        parentRequestId: task.parentRequestId,
      }
    },
      async (span) => {
        await this.#processMoudleStream(task, stream);
      }
    );
  }

  async #processMoudleStream(task: MoudleFragmentTask, stream: ApiStream): Promise<void> {
    const htmlParser = new StreamHtmlParser(task.parentRequestId);
    const inputPromise = (async () => {
      for await (const chunk of stream) {
        htmlParser.parseChunk(chunk);
      }
      htmlParser.endStream();
    })();
    for await (const item of htmlParser.stream()) {
      if (item.eventType === 'append') {
        item.data.requestId = task.requestId;
        item.data.parentRequestId = task.parentRequestId;
      }
      this.#emitData('append', item.data);
    }
    await inputPromise;
  }

  #emitData(event: string, data: EmitData) {
    this.emit(event, data);
  }
}
