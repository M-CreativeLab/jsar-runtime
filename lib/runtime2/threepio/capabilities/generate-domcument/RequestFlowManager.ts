import { EventEmitter } from 'events';
import {
  EmitData,
  FragmentType,
  MoudleFragment,
  ParsedHeader,
  ParsedModule,
  MoudleParserEventType,
  MoudleFragmentTask,
} from './interfaces';
import { createModuleTask } from './taskDecomposer';
import { callLLM } from '../../utils/llmClient';
import { generateStructuralStream } from './htmlStructuralGenerator';
import { reportThreepioError, reportThreepioInfo } from '../../utils/threepioLog';
import { getPlanPrompt } from './prompts';
import { StreamPlannerParser } from './parsers/jsonl/StreamPlannerParser';
import { ApiStream } from '../../api/transform/stream';

export interface RequestFlowManager {
  on(event: MoudleParserEventType, listener: (data: EmitData) => void): this;
}

export class RequestFlowManager extends EventEmitter {
  public async executeFlow(input: string): Promise<void> {
    let headerParsed = false;
    let taskPromises: Promise<void>[] = [];
    const plannerParser = new StreamPlannerParser();
    const systemPrompt = getPlanPrompt();
    try {
      const stream = callLLM(input, systemPrompt);
      await this.#processPlannerStream(stream, plannerParser, taskPromises, () => { headerParsed = true; });
      await Promise.all(taskPromises);
      reportThreepioInfo('All tasks completed.');
    } catch (error) {
      reportThreepioError('Error during flow execution:', error);
      throw error;
    }
  }

  async #processPlannerStream(
    stream: ApiStream,
    plannerParser: StreamPlannerParser,
    taskPromises: Promise<void>[],
    onHeaderParsed: () => void
  ) {
    const parserStreamPromise = (async () => {
      for await (const item of plannerParser.stream()) {
        switch (item.type) {
          case 'header':
            const header = item.data as ParsedHeader;
            onHeaderParsed();
            const layout = header.layout.replace(/height/g, 'min-height');
            reportThreepioInfo(`Header parsed layout: ${layout}`);
            this.#emitData('append', { type: FragmentType.Header, fragment: { content: layout } });
            break;

          case 'module':
            const module = item.data as ParsedModule;
            if (!onHeaderParsed) {
              reportThreepioError('Module parsed before root node was created. Aborting.');
              break;
            }
            const moduleParentId = 'moudle' + taskPromises.length;
            module.parentId = moduleParentId;
            const task = createModuleTask(module, '', moduleParentId);
            this.#emitData('append', {
              type: FragmentType.Moudle,
              fragment: { id: moduleParentId, content: module.layout } as MoudleFragment
            });

            const p = this.#generateModuleFragments(task);
            taskPromises.push(p);
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
    })();

    const inputPromise = (async () => {
      for await (const chunk of stream) {
        if (chunk.type === 'text') {
          plannerParser.parseTextChunk(chunk);
        }
      }
      plannerParser.endStream();
    })();

    await Promise.all([parserStreamPromise, inputPromise]);
  }

  async #generateModuleFragments(task: MoudleFragmentTask): Promise<void> {
    try {
      reportThreepioInfo(`Generating fragment for task: ${task.moudle.name}`);

      for await (const fragment of generateStructuralStream(task)) {
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

  #emitData(event: string, data: EmitData) {
    this.emit(event, data);
  }
}
