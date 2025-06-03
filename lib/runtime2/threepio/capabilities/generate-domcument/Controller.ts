// import { EventEmitter } from 'events';
// import {
//   EmitData,
//   EmitterEventType,
//   FragmentType,
//   ParsedHeader,
//   ParsedModule,
//   LLMAPI,
//   MoudleFragment
// } from './interfaces';
// import { StreamPlannerParser } from './parsers/StreamPlannerParser';
// import { createModuleNodeAndTask } from './TaskDecomposer';
// import { PLANNER_PROMPT } from './prompts/planner.prompt';
// import { callLLM } from '../../utils/llm';
// import { generateFragmentStream } from './HTMLFragmentGenerator';

// export interface Controller {
//   on(event: 'append', listener: (data: EmitData) => void): this;
// }

// class PlannerLLMAPI implements LLMAPI {
//   sendPromptStream(input) {
//     const emitter = new EventEmitter();
//     const systemPrompt = PLANNER_PROMPT;
//     const stream = callLLM(input, systemPrompt);
//     this.handleStream(stream, emitter);
//     return emitter;
//   }

//   async handleStream(stream, emt: EventEmitter) {
//     for await (const chunk of await stream) {
//       emt.emit('chunk', chunk);
//       if (chunk.type === 'text') {
//         emt.emit('data', chunk.text);
//       } else if (chunk.type === 'reasoning') {
//         console.log('reasoning', chunk.reasoning);
//       } else if (chunk.type === 'usage') {
//         emt.emit('end');
//       }
//     }
//   }
// }

// export class Controller extends EventEmitter {
//   #plannerLLM: LLMAPI;

//   constructor() {
//     super();
//     this.#plannerLLM = new PlannerLLMAPI();
//   }

//   public async generatePageStream(input: string) {
//     let taskPromises: Promise<any>[] = [];
//     let headerParsed = false;
//     let overallDesignTheme = '';
//     let appName = '';

//     const plannerParser = new StreamPlannerParser();
//     const plannerStreamPromise = new Promise<void>((resolvePlannerPhase, rejectPlannerPhase) => {
//       plannerParser.on('headerParsed', (header: ParsedHeader) => {
//         overallDesignTheme = header.overallTheme;
//         appName = header.appName;
//         headerParsed = true;
//         const layout = header.layout.replace(/height/g, 'min-height');
//         console.log(`Header parsed with layout: ${layout}`);
//         this.#emitData(EmitterEventType.append, { type: FragmentType.Header, fragment: { content: layout } });
//       });

//       plannerParser.on('moduleParsed', (module: ParsedModule) => {
//         if (!headerParsed) {
//           console.error('Planner: Module parsed before root node was created. Aborting.');
//           rejectPlannerPhase(new Error('Module received before header was fully processed.'));
//           return;
//         }
//         const mourdleParentId = 'moudle' + taskPromises.length;
//         const { allTasks } = createModuleNodeAndTask(module, overallDesignTheme, mourdleParentId);
//         this.#emitData(EmitterEventType.append, { type: FragmentType.Moudle, fragment: { id: module.parentId, content: module.layout } as MoudleFragment });
//         allTasks.forEach(task => {
//           const p = (async () => {
//             try {
//               console.log(`Generating fragment for task: ${task.moudle.name}`);
//               await generateFragmentStream(task, this);
//             } catch (error) {
//               console.error(`Error generating fragment for task ${task}:`, error);
//             }
//           })();
//           taskPromises.push(p);
//         });
//       });

//       plannerParser.on('parseEnd', () => {
//         Promise.all(taskPromises).then(() => {
//           resolvePlannerPhase();
//         }).catch(rejectPlannerPhase);
//       });

//       plannerParser.on('error', (err: Error) => {
//         console.error('  Planner: Parsing error:', err);
//         rejectPlannerPhase(err);
//       });

//       const plannerLLMStream = this.#plannerLLM.sendPromptStream(input);
//       plannerLLMStream.on('chunk', (chunk: any) => plannerParser.parseChunk(chunk));
//       plannerLLMStream.on('error', (err: Error) => rejectPlannerPhase(err));
//     });
//     await plannerStreamPromise;
//   }

//   #emitData(event: string, data: EmitData) {
//     this.emit(event, data);
//   }
// }
