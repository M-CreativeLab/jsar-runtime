import { EventEmitter } from 'events';
import {
  EmitData,
  EmitterEventType,
  FragmentType,
  ParsedHeader,
  ParsedModule,
  MoudleFragment
} from './interfaces';
import { StreamPlannerParser } from './parsers/StreamPlannerParser';
import { createModuleTask } from './taskDecomposer';
import { PLANNER_PROMPT } from './prompts/planner.prompt';
import { callLLM } from '../../utils/llmClient';
import { generateStructuralStream } from './htmlStructuralGenerator';

export interface RequestFlowManager {
  on(event: 'append', listener: (data: EmitData) => void): this;
}

export class RequestFlowManager extends EventEmitter {

  constructor() {
    super();
  }

  public async executeFlow(input: string): Promise<void> {
    let headerParsed = false;
    let taskPromises: Promise<any>[] = [];
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
    taskPromises: Promise<any>[],
    onHeaderParsed: () => void,
    resolvePlannerPhase: () => void,
    rejectPlannerPhase: (err: Error) => void
  ) {
    plannerParser.on('headerParsed', (header: ParsedHeader) => {
      onHeaderParsed();
      const layout = header.layout.replace(/height/g, 'min-height');
      console.log(`Header parsed with layout: ${layout}`);
      this.#emitData(EmitterEventType.append, { type: FragmentType.Header, fragment: { content: layout } });
    });
    plannerParser.on('moduleParsed', (module: ParsedModule) => {
      if (!onHeaderParsed) {
        console.error('Planner: Module parsed before root node was created. Aborting.');
        return;
      }
      const mourdleParentId = 'moudle' + taskPromises.length;
      const { allTasks } = createModuleTask(module, '', mourdleParentId);
      this.#emitData(EmitterEventType.append, { type: FragmentType.Moudle, fragment: { id: mourdleParentId, content: module.layout } as MoudleFragment });
      allTasks.forEach(task => {
        const p = (async () => {
          try {
            console.log(`Generating fragment for task: ${task.moudle.name}`);
            await generateStructuralStream(task, { onData: this.#onMoudleData.bind(this), onError: this.#onMoudleError.bind(this) });
          } catch (error) {
            console.error(`Error generating fragment forƒ task ${task}:`, error);
          }
        })();
        taskPromises.push(p);
      });
    });
    plannerParser.on('parseEnd', () => {
      console.log('Planner stage completed.');
      Promise.all(taskPromises).then(() => {
        console.log('All tasks completed.');
        resolvePlannerPhase();
      }).catch(rejectPlannerPhase);
    });
    plannerParser.on('error', () => {
      console.error('Planner: Parsing error occurred.');
      rejectPlannerPhase(new Error('Error during planner parsing.'));
    });
  }

  async #processPlannerStream(stream: any, plannerParser: StreamPlannerParser) {
    for await (const chunk of await stream) {
      plannerParser.parseChunk(chunk);
    }
    plannerParser.end();
  }

  #onMoudleData(eventTye: string, data: EmitData) {
    if (eventTye === EmitterEventType.append) {
      this.emit(EmitterEventType.append, data);
    }
  }

  #onMoudleError(error: Error) {
    console.error('Error in Moudle processing:', error);
  }

  #emitData(event: string, data: EmitData) {
    console.log(`Emitting event: ${event} with data:`, data);
    this.emit(event, data);
  }
}
