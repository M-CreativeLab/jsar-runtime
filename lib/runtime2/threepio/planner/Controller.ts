import { EventEmitter } from 'events';
import {
  EmitData,
  EmitterEventType,
  FragmentType,
  ParsedHeader,
  ParsedModule,
  PerformanceType,
  LLMAPI
} from './interfaces';
import { StreamPlannerParser } from './parse/StreamPlannerParser';
import { TaskDecomposer } from './TaskDecomposer';
import { HtmlFragmentGenerator } from './HTMLFragmentGenerator';
import { PerformanceTracer } from '../utils/PerformanceTracer';
import { emitDataFun } from '../utils/emitUtils';
import { PLANNER_PROMPT } from './prompt/planner.prompt';
import { callLLM } from './llmClient';

export function createController(tracker: PerformanceTracer): Controller {
  const plannerLLMAPI = new PlannerLLMAPI();
  return new Controller(plannerLLMAPI, tracker);
}
class PlannerLLMAPI implements LLMAPI {
  sendPromptStream(input) {
    const emitter = new EventEmitter();
    const systemPrompt = PLANNER_PROMPT;
    const stream = callLLM(input, systemPrompt);
    this.handleStream(stream, emitter);
    return emitter;
  }

  async handleStream(stream, emt: EventEmitter) {
    for await (const chunk of await stream) {
      emt.emit('chunk', chunk);
      if (chunk.type === 'text') {
        emt.emit('data', chunk.text);
      } else if (chunk.type === 'reasoning') {
        console.log('reasoning', chunk.reasoning);
      } else if (chunk.type === 'usage') {
        emt.emit('end');
      }
    }
  }
}

export class Controller extends EventEmitter {
  #decomposer: TaskDecomposer;
  #generator: HtmlFragmentGenerator;
  #plannerLLM: LLMAPI;
  #tracker: PerformanceTracer;

  constructor(plannerLLM: PlannerLLMAPI, tracker?: PerformanceTracer) {
    super();
    this.#decomposer = new TaskDecomposer();
    this.#generator = new HtmlFragmentGenerator();
    this.#plannerLLM = plannerLLM;
    this.#tracker = tracker || new PerformanceTracer();
  }

  public async generatePageStream(input: string) {
    const totalTaskID = this.#tracker.start(PerformanceType.total);
    const plannerTaskID = this.#tracker.start(PerformanceType.planner);
    const parseTaskID = this.#tracker.start(PerformanceType.parseHeader);
    this.#tracker.start(PerformanceType.parseMoudle);
    let taskPromises: Promise<any>[] = [];
    let headerParsed = false;
    let overallDesignTheme = '';
    let appName = '';

    const plannerParser = new StreamPlannerParser(this);
    const plannerStreamPromise = new Promise<void>((resolvePlannerPhase, rejectPlannerPhase) => {
      this.on('headerParsed', (header: ParsedHeader) => {
        this.#tracker.end(parseTaskID);
        overallDesignTheme = header.overallTheme;
        appName = header.appName;
        headerParsed = true;
        let layout = header.layout.replace(/height/g, 'min-height');
        console.log(`Header parsed with layout: ${layout}`);
        this.#emitData(EmitterEventType.append, { type: FragmentType.Header, fragment: { content: layout } });
      });

      this.on('moduleParsed', (module: ParsedModule) => {
        if (!headerParsed) {
          console.error('Planner: Module parsed before root node was created. Aborting.');
          rejectPlannerPhase(new Error('Module received before header was fully processed.'));
          return;
        }
        this.#tracker.end(PerformanceType.parseMoudle);
        const mourdleParentId = 'moudle' + taskPromises.length;
        const { allTasks } = this.#decomposer.createModuleNodeAndTask(module, overallDesignTheme, mourdleParentId);
        this.#emitData(EmitterEventType.append, { type: FragmentType.Moudle, fragment: { id: module.parentId, content: module.layout } });
        allTasks.forEach(task => {
          const p = (async () => {
            try {
              console.log(`Generating fragment for task: ${task.moudle.name}`);
              const taskID = this.#tracker.start(task.moudle.name);
              await this.#generator.generateFragmentStream(task, this);
              this.#tracker.end(taskID);
            } catch (error) {
              console.error(`Error generating fragment for task ${task}:`, error);
            }
          })();
          taskPromises.push(p);
        });
      });

      this.on('parseEnd', () => {
        this.#tracker.end(plannerTaskID);
        Promise.all(taskPromises).then(() => {
          resolvePlannerPhase();
        }).catch(rejectPlannerPhase);
      });

      this.on('error', (err: Error) => {
        console.error('  Planner: Parsing error:', err);
        rejectPlannerPhase(err);
      });

      const plannerLLMStream = this.#plannerLLM.sendPromptStream(input);
      plannerLLMStream.on('chunk', (chunk: any) => plannerParser.parseChunk(chunk));
      plannerLLMStream.on('error', (err: Error) => rejectPlannerPhase(err));
    });
    await plannerStreamPromise;
    this.#tracker.end(totalTaskID);
    this.#tracker.report();
  }

  #emitData(event: string, data: EmitData) {
    emitDataFun(this, event, data);
  }
}
