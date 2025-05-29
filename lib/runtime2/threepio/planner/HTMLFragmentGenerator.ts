import { EventEmitter } from 'events';
import { MoudleFragmentTask } from './interfaces';
import { callLLM } from './llmClient';
import { HTML_FRAGMENT_TEMPLATE_PROMPT } from './prompt/worker.prompt';
import { StreamHtmlParser } from './parse/StreamHtmlParser';

export class HtmlFragmentGenerator {
  /**
   * Creates a prompt for generating HTML fragments based on the provided task.
   * The prompt includes the page goal, parent ID, and design system information.
   * @param task - The MoudleFragmentTask containing context and module information.
   * @returns An object containing the input JSON string and the formatted prompt string.
   * @throws Will throw an error if the task context is not provided.
   */
  private createFragmentGeneratorPrompt(task: MoudleFragmentTask): { input: string, prompt: string } {
    const context = task.context;
    let prompt = HTML_FRAGMENT_TEMPLATE_PROMPT;
    prompt = prompt.replace(/{{PAGE_GOAL}}/g, context?.pageGoal || '');
    prompt = prompt.replace(/{{PARENT_ID}}/g, task.moudle?.parentId || '');
    prompt = prompt.replace(/{{DESIGN_SYSTEM_INFO}}/g, context?.designSystemInfo || '');
    return { input: JSON.stringify(task.moudle), prompt: prompt }
  }

  public async generateFragmentStream(task: MoudleFragmentTask, emt: EventEmitter): Promise<void> {
    const { input, prompt } = this.createFragmentGeneratorPrompt(task);
    const htmlParser = new StreamHtmlParser(task.moudle.name, emt);
    try {
      const stream = callLLM(input, prompt);
      console.log(`Calling LLM with input: ${input}  prompt: ${prompt}`);
      console.log(`LLM Response:`, task.context.designSystemInfo);
      for await (const chunk of await stream) {
        htmlParser.parseChunk(chunk);
      }
    } catch (error) {
      console.error(`Error generating ${task.fragmentType}`, error);
    }
  }
}
