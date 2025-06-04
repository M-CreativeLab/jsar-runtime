import { MoudleFragmentTask, StreamHtmlParserCallbacks } from './interfaces';
import { HTML_FRAGMENT_TEMPLATE_PROMPT } from './prompts/worker.prompt';
import { StreamHtmlParser } from './parsers/StreamHtmlParser';
import { callLLM } from '../../utils/llmClient';
import { threepioError, threepioLog } from '../../utils/threepioLog';

/**
 *  Generates a stream of HTML fragments based on the provided task and callbacks.
 * @param task 
 * @param callbacks 
 */
export async function generateStructuralStream(task: MoudleFragmentTask, callbacks: StreamHtmlParserCallbacks): Promise<void> {
  const prompt = createPrompt(task);
  const input = JSON.stringify(task.moudle);
  const htmlParser = new StreamHtmlParser(task.moudle.name, callbacks);
  try {
    threepioLog(`Calling LLM with input: ${input}  prompt: ${prompt.substring(0, 100)}...`);
    const stream = await callLLM(input, prompt);
    for await (const chunk of stream) {
      htmlParser.parseChunk(chunk);
    }
  } catch (error) {
    threepioError(`Error generating ${task.fragmentType}`, error);
    callbacks.onError?.(error);
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
