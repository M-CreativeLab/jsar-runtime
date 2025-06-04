import { EventEmitter } from 'events';
import {
  EmitData,
  FragmentType,
  MoudleFragment,
  ParsedHeader,
  ParsedModule,
  MoudleParserEventType,
} from './interfaces';
import { StreamPlannerParser } from './parsers/StreamPlannerParser';
import { createModuleTask } from './TaskDecomposer';
import { PLANNER_PROMPT } from './prompts/planner.prompt';
import { callLLM } from '../../utils/llmClient';
import { generateStructuralStream } from './htmlStructuralGenerator';
import { reportThreepioError, reportThreepioInfo } from '../../utils/threepioLog';

export interface RequestFlowManager {
  on(event: MoudleParserEventType, listener: (data: EmitData) => void): this;
}

export class RequestFlowManager extends EventEmitter {
  public async executeFlow(input: string): Promise<void> {
    let headerParsed = false;
    let taskPromises: Promise<void>[] = [];
    const plannerParser = new StreamPlannerParser();
    const systemPrompt = PLANNER_PROMPT;
    const stream = callLLM(input, systemPrompt);
    const plannerStreamPromise = new Promise<void>((resolvePlannerPhase, rejectPlannerPhase) => {
      this.#registerPlannerParserHandlers(plannerParser, taskPromises, () => { headerParsed = true; }, resolvePlannerPhase, rejectPlannerPhase);
      this.#processPlannerStream(stream, plannerParser);
    });
    await plannerStreamPromise;
  }

  #registerPlannerParserHandlers(
    plannerParser: StreamPlannerParser,
    taskPromises: Promise<void>[],
    onHeaderParsed: () => void,
    resolvePlannerPhase: () => void,
    rejectPlannerPhase: (err: Error) => void
  ) {

    plannerParser.on('headerParsed', (header: ParsedHeader) => {
      onHeaderParsed();
      const layout = header.layout.replace(/height/g, 'min-height');
      reportThreepioInfo(`Header parsed with layout: ${layout}`);
      this.#emitData('append', { type: FragmentType.Header, fragment: { content: layout } });
    });

    plannerParser.on('moduleParsed', (module: ParsedModule) => {
      if (!onHeaderParsed) {
        reportThreepioError('Planner: Module parsed before root node was created. Aborting.');
        return;
      }
      const mourdleParentId = 'moudle' + taskPromises.length;
      module.parentId = mourdleParentId;
      const task = createModuleTask(module, '', mourdleParentId);
      this.#emitData('append', { type: FragmentType.Moudle, fragment: { id: mourdleParentId, content: module.layout } as MoudleFragment });
      const p = (async () => {
        try {
          reportThreepioInfo(`Generating fragment for task: ${task.moudle.name}`);
          for await (const fragment of generateStructuralStream(task)) {
            if (fragment.eventType === 'append') {
              this.#emitData('append', fragment.data);
            }
          }
        } catch (error) {
          reportThreepioError(`Error generating fragment for task ${task}:`, error);
        }
      })();
      taskPromises.push(p);
    });

    plannerParser.on('parseEnd', (data) => {
      console.log('Planner parsing completed successfully.', data);
      Promise.all(taskPromises).then(() => {
        reportThreepioInfo('All tasks completed.');
        resolvePlannerPhase();
      }).catch(rejectPlannerPhase);
    });

    plannerParser.on('error', () => {
      reportThreepioError('Planner: Parsing error occurred.');
      rejectPlannerPhase(new Error('Error during planner parsing.'));
    });
  }

  async #processPlannerStream(stream: any, plannerParser: StreamPlannerParser) {
    reportThreepioInfo('Processing planner stream...');
    for await (const chunk of await stream) {
      reportThreepioInfo('Received chunk from planner stream:', chunk);
      if (chunk.type === 'text') {
        plannerParser.parseTextChunk(chunk);
      }
    }
    reportThreepioInfo('Processing planner stream ended.');
    plannerParser.end();
  }

  #emitData(event: string, data: EmitData) {
    this.emit(event, data);
  }
}
