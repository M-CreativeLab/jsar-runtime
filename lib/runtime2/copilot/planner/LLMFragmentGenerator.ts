import { LLMMoudleFragmentTask } from './interfaces';
import { callLLMStream } from './LLMClient';
import { HTML_FRAGMENT_PROMPT } from './prompt/workerPrompt';
import { StreamHtmlParser } from './parse/StreamHtmlParser';
import { EventEmitter } from 'events';


export class LLMFragmentGenerator {

  /**
   * Constructs the prompt for the LLM based on the task context.
   * @param task The LLMFragmentTask containing node and context info.
   * @returns The constructed prompt string.
   */
  private createFragmentGeneratorPrompt(task: LLMMoudleFragmentTask): { input: string, prompt: string } {
    const context = task.context;
    console.log(`LLMFragmentGenerator inputContext:`, JSON.stringify(task));
    let promptTemplate = HTML_FRAGMENT_PROMPT;
    promptTemplate = promptTemplate.replace(/{{PAGE_GOAL}}/g, context?.pageGoal || '');
    promptTemplate = promptTemplate.replace(/{{PARENT_ID}}/g, task.moudle?.parentId || '');
    promptTemplate = promptTemplate.replace(/{{DESIGN_SYSTEM_INFO}}/g, context?.designSystemInfo || '');
    // promptTemplate = promptTemplate.replace(/\s/g, '');
    return { input: JSON.stringify(task.moudle), prompt: promptTemplate }
  }

  public async generateFragmentStream(task: LLMMoudleFragmentTask, emt: EventEmitter): Promise<void> {
    const { input, prompt } = this.createFragmentGeneratorPrompt(task);
    const htmlParse = new StreamHtmlParser(task.moudle.name, emt);
    try {
      const stream = callLLMStream(input, prompt);
      console.log(`Calling LLM with input: ${input}  prompt: ${prompt}`);
      console.log(`LLM Response:`, task.context.designSystemInfo);
      for await (const chunk of await stream) {
        htmlParse.processChunk(chunk);
      }
    } catch (error: any) {
      console.error(`Error generating ${task.fragmentType}`, error);
    }
  }
}