import { Capability } from './capabilities/interface';
import { GenerateDocumentCapability } from './capabilities/generate-domcument';
import { ThreepioPromptIntegration } from './prompt-api-integration';

export class Threepio {
  #capabilities: Capability[] = [];
  #promptIntegration: ThreepioPromptIntegration;

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#capabilities.push(new GenerateDocumentCapability(browsingContext));
    this.#promptIntegration = new ThreepioPromptIntegration();
  }

  /**
   * Legacy request method - maintains backward compatibility
   */
  public async request(input: string): Promise<void> {
    const defaultCapability = this.#capabilities[0];
    if (defaultCapability) {
      await defaultCapability.request(input);
    }
  }

  /**
   * New Prompt API-based content generation
   * Returns the generated content as a string instead of modifying the DOM directly
   */
  public async generateContent(input: string): Promise<string> {
    return await this.#promptIntegration.generateContent(input);
  }

  /**
   * Stream content generation using the Prompt API
   */
  public async streamContent(input: string): Promise<ReadableStream<string>> {
    return await this.#promptIntegration.streamContent(input);
  }

  /**
   * Get session statistics
   */
  public getSessionStats() {
    return this.#promptIntegration.getSessionStats();
  }

  /**
   * Create a parallel session for concurrent operations
   */
  public async createParallelSession() {
    return await this.#promptIntegration.createParallelSession();
  }

  /**
   * Clean up resources
   */
  public destroy(): void {
    this.#promptIntegration.destroy();
  }
}
