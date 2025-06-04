import { EmitData, MoudleFragmentTask, MoudleParserEventType, StreamHtmlParserCallbacks } from './interfaces';
import { HTML_FRAGMENT_TEMPLATE_PROMPT } from './prompts/worker.prompt';
import { StreamHtmlParser } from './parsers/StreamHtmlParser';
import { callLLM } from '../../utils/llmClient';
import { threepioLog } from '../../utils/threepioLog';

interface MoudleStructuralStream {
  eventType: MoudleParserEventType,
  data: EmitData
}
/**
 *  Generates a stream of HTML fragments based on the provided task.
 *  Now returns an async generator yielding each parsed fragment.
 */
export async function* generateStructuralStream(task: MoudleFragmentTask): AsyncGenerator<MoudleStructuralStream, void, unknown> {
  const prompt = createPrompt(task);
  const input = JSON.stringify(task.moudle);
  // collecting data in a queue to yield later
  const queue: MoudleStructuralStream[] = [];
  let resolve: (() => void) | null = null;
  let done = false;

  const callbacks: StreamHtmlParserCallbacks = {
    onData: (eventType, data) => {
      queue.push({ eventType, data });
      if (resolve) resolve();
    },
    onStreamEnd: () => {
      done = true;
      if (resolve) resolve();
    }
  };

  const htmlParser = new StreamHtmlParser(task.moudle.name, callbacks);
  threepioLog(`Calling LLM with input: ${input}  prompt: ${prompt.substring(0, 100)}...`);
  const stream = await callLLM(input, prompt);
  (async () => {
    for await (const chunk of await stream) {
      if (chunk.type === 'text') {
        htmlParser.parseTextChunk(chunk);
      }
    }
    htmlParser.endStream();
  })();

  while (!done || queue.length > 0) {
    if (queue.length === 0) {
      await new Promise<void>(r => (resolve = r));
      resolve = null;
    }
    while (queue.length > 0) {
      yield queue.shift();
    }
  }
}

/**
 * Creates a prompt for generating HTML fragments based on the provided task.
 * The prompt includes the page goal, parent ID, and design system information.
 * @param task - The MoudleFragmentTask containing context and module information.
 * @returns An object containing the input JSON string and the formatted prompt string.
 * @throws Will throw an error if the task context is not provided.
 */
function createPrompt(task: MoudleFragmentTask): string {
  const context = task.context;
  let prompt = HTML_FRAGMENT_TEMPLATE_PROMPT;
  prompt = prompt.replace(/{{PAGE_GOAL}}/g, context?.pageGoal || '');
  prompt = prompt.replace(/{{PARENT_ID}}/g, task.moudle?.parentId || '');
  prompt = prompt.replace(/{{DESIGN_SYSTEM_INFO}}/g, context?.designSystemInfo || '');
  return prompt;
}
