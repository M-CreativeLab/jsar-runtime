import { MoudleFragmentTask } from './interfaces';
import { callLLM } from '../../utils/llmClient';
import { reportThreepioInfo } from '../../utils/threepioLog';
import { getWorkerPrompt } from './prompts';
import { HtmlStreamItem, StreamHtmlParser } from './parsers/jsonl/StreamHtmlParser';

export async function* generateStructuralStream(task: MoudleFragmentTask): AsyncGenerator<HtmlStreamItem, void, unknown> {
  const prompt = createPrompt(task);
  const input = JSON.stringify(task.moudle);
  const htmlParser = new StreamHtmlParser(task.moudle.name);

  reportThreepioInfo(`Calling LLM with input: ${input} prompt: ${prompt.substring(0, 100)}...`);
  const stream = await callLLM(input, prompt);

  const parserStreamPromise = (async function* () {
    for await (const item of htmlParser.stream()) {
      yield item;
    }
  })();

  const inputPromise = (async () => {
    for await (const chunk of stream) {
      if (chunk.type === 'text') {
        htmlParser.parseTextChunk(chunk);
      }
    }
    htmlParser.endStream();
  })();

  yield* parserStreamPromise;
  await inputPromise;
}

/**
 * Creates a prompt for generating HTML fragments based on the provided task.
 * The prompt includes the page goal, parent ID, and design system information.
 * @param task - The MoudleFragmentTask containing context and module information.
 * @returns An object containing the input JSON string and the formatted prompt string.
 */
function createPrompt(task: MoudleFragmentTask): string {
  const context = task.context;
  let prompt = getWorkerPrompt();
  prompt = prompt.replace(/{{PAGE_GOAL}}/g, context?.pageGoal || '');
  prompt = prompt.replace(/{{PARENT_ID}}/g, task.moudle?.parentId || '');
  prompt = prompt.replace(/{{DESIGN_SYSTEM_INFO}}/g, context?.designSystemInfo || '');
  return prompt;
}
