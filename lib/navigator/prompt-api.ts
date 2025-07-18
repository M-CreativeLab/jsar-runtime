import {
  LanguageModelStatic,
  LanguageModelSession,
  LanguageModelCreateOptions,
  LanguageModelAvailabilityOptions,
  LanguageModelAvailability,
  LanguageModelParams,
  LanguageModelCloneOptions,
  PromptMessage,
  PromptOptions,
  QuotaExceededError,
  LanguageModelMonitor,
} from './prompt-api-types';
import { bridgeToExistingLLM } from './llm-bridge';

class LanguageModelMonitorImpl extends EventTarget implements LanguageModelMonitor {
  constructor() {
    super();
    // Simulate initial download progress events
    setTimeout(() => {
      this.dispatchEvent(new ProgressEvent('downloadprogress', { loaded: 0, total: 1 }));
    }, 10);
    setTimeout(() => {
      this.dispatchEvent(new ProgressEvent('downloadprogress', { loaded: 1, total: 1 }));
    }, 100);
  }
}

class LanguageModelSessionImpl extends EventTarget implements LanguageModelSession {
  private _destroyed = false;
  private _conversation: PromptMessage[] = [];
  private _initialPrompts: PromptMessage[] = [];
  private _temperature?: number;
  private _topK?: number;
  private _inputUsage = 0;
  private _inputQuota = 100000; // Default quota, arbitrary value

  constructor(options: LanguageModelCreateOptions = {}) {
    super();
    
    if (options.initialPrompts) {
      this._initialPrompts = [...options.initialPrompts];
      this._conversation = [...options.initialPrompts];
    }
    
    this._temperature = options.temperature;
    this._topK = options.topK;
    
    // Validate system prompt position
    const systemPromptIndex = this._conversation.findIndex(msg => msg.role === 'system');
    if (systemPromptIndex !== -1 && systemPromptIndex !== 0) {
      throw new TypeError('System prompt must be at position 0 in initialPrompts');
    }
  }

  get inputUsage(): number {
    return this._inputUsage;
  }

  get inputQuota(): number {
    return this._inputQuota;
  }

  async prompt(input: string | PromptMessage[], options: PromptOptions = {}): Promise<string> {
    if (this._destroyed) {
      throw new DOMException('Session has been destroyed', 'AbortError');
    }

    const messages = this._normalizeInput(input);
    const usage = await this.measureInputUsage(messages, options);
    
    // Check quota
    if (usage + this._inputUsage > this._inputQuota) {
      // Try to make space by removing old messages (except system prompt)
      this._handleOverflow(usage);
    }

    // Add the new messages to conversation
    this._conversation.push(...messages);
    this._inputUsage += usage;

    try {
      const systemPrompt = this._getSystemPrompt();
      const conversationText = this._formatConversationForLLM();
      
      console.log('LanguageModel: Starting prompt with input:', conversationText);
      
      // Use the LLM bridge to connect to existing infrastructure
      const stream = bridgeToExistingLLM({
        input: conversationText,
        systemPrompt: systemPrompt
      });

      let response = '';
      for await (const chunk of stream) {
        if (options.signal?.aborted) {
          throw new DOMException('Operation was aborted', 'AbortError');
        }
        
        if (chunk.type === 'text' && chunk.text) {
          response += chunk.text;
        } else if (chunk.type === 'error') {
          throw new Error(chunk.error?.message || 'Unknown error');
        }
      }

      // Add assistant response to conversation
      this._conversation.push({ role: 'assistant', content: response.trim() });
      
      return response.trim();
    } catch (error) {
      console.error('LanguageModel: Error in prompt:', error);
      throw error;
    }
  }

  async promptStreaming(input: string | PromptMessage[], options: PromptOptions = {}): Promise<ReadableStream<string>> {
    if (this._destroyed) {
      throw new DOMException('Session has been destroyed', 'AbortError');
    }

    const messages = this._normalizeInput(input);
    const usage = await this.measureInputUsage(messages, options);
    
    // Check quota
    if (usage + this._inputUsage > this._inputQuota) {
      this._handleOverflow(usage);
    }

    this._conversation.push(...messages);
    this._inputUsage += usage;

    const systemPrompt = this._getSystemPrompt();
    const conversationText = this._formatConversationForLLM();
    
    return new ReadableStream<string>({
      async start(controller) {
        let response = '';
        try {
          // Use the LLM bridge for streaming
          const stream = bridgeToExistingLLM({
            input: conversationText,
            systemPrompt: systemPrompt
          });
          
          for await (const chunk of stream) {
            if (options.signal?.aborted) {
              controller.error(new DOMException('Operation was aborted', 'AbortError'));
              return;
            }
            
            if (chunk.type === 'text' && chunk.text) {
              response += chunk.text;
              controller.enqueue(chunk.text);
            } else if (chunk.type === 'error') {
              controller.error(new Error(chunk.error?.message || 'Unknown error'));
              return;
            }
          }
          
          // Add complete response to conversation
          if (response.trim()) {
            this._conversation.push({ role: 'assistant', content: response.trim() });
          }
          controller.close();
        } catch (error) {
          controller.error(error);
        }
      }.bind(this)
    });
  }

  async append(input: string | PromptMessage[], options: { signal?: AbortSignal } = {}): Promise<void> {
    if (this._destroyed) {
      throw new DOMException('Session has been destroyed', 'AbortError');
    }

    const messages = this._normalizeInput(input);
    const usage = await this.measureInputUsage(messages, options);
    
    if (usage + this._inputUsage > this._inputQuota) {
      this._handleOverflow(usage);
    }

    this._conversation.push(...messages);
    this._inputUsage += usage;
  }

  async measureInputUsage(input: string | PromptMessage[], options: { signal?: AbortSignal } = {}): Promise<number> {
    const messages = this._normalizeInput(input);
    
    // Simple token estimation - in a real implementation this would use proper tokenization
    let totalChars = 0;
    for (const message of messages) {
      if (typeof message.content === 'string') {
        totalChars += message.content.length;
      } else {
        // For multimodal content, estimate based on type
        for (const content of message.content) {
          if (content.type === 'text' && typeof content.value === 'string') {
            totalChars += content.value.length;
          } else {
            // Rough estimate for non-text content
            totalChars += 1000;
          }
        }
      }
    }
    
    // Rough approximation: 4 characters per token
    return Math.ceil(totalChars / 4);
  }

  async clone(options: LanguageModelCloneOptions = {}): Promise<LanguageModelSession> {
    if (this._destroyed) {
      throw new DOMException('Session has been destroyed', 'AbortError');
    }

    const cloneOptions: LanguageModelCreateOptions = {
      initialPrompts: [...this._initialPrompts],
      temperature: this._temperature,
      topK: this._topK,
      signal: options.signal,
    };

    return new LanguageModelSessionImpl(cloneOptions);
  }

  destroy(): void {
    this._destroyed = true;
    this._conversation = [];
    this.dispatchEvent(new Event('destroy'));
  }

  private _normalizeInput(input: string | PromptMessage[]): PromptMessage[] {
    if (typeof input === 'string') {
      return [{ role: 'user', content: input }];
    }
    return input;
  }

  private _getSystemPrompt(): string | null {
    const systemMessage = this._conversation.find(msg => msg.role === 'system');
    return systemMessage && typeof systemMessage.content === 'string' ? systemMessage.content : null;
  }

  private _formatConversationForLLM(): string {
    // Format conversation for the existing LLM client
    // Skip system prompt as it's handled separately
    const userMessages = this._conversation.filter(msg => msg.role !== 'system');
    return userMessages.map(msg => {
      if (typeof msg.content === 'string') {
        return `${msg.role}: ${msg.content}`;
      } else {
        // For multimodal content, extract text parts for now
        const textParts = msg.content
          .filter(c => c.type === 'text' && typeof c.value === 'string')
          .map(c => c.value)
          .join(' ');
        return `${msg.role}: ${textParts}`;
      }
    }).join('\n');
  }

  private _handleOverflow(requiredSpace: number): void {
    const neededSpace = requiredSpace + this._inputUsage - this._inputQuota;
    let freedSpace = 0;
    
    // Remove messages from the beginning (except system prompt)
    const systemPromptIndex = this._conversation.findIndex(msg => msg.role === 'system');
    let removeIndex = systemPromptIndex === 0 ? 1 : 0;
    
    while (freedSpace < neededSpace && removeIndex < this._conversation.length) {
      const removedMessage = this._conversation[removeIndex];
      if (typeof removedMessage.content === 'string') {
        freedSpace += Math.ceil(removedMessage.content.length / 4);
      }
      this._conversation.splice(removeIndex, 1);
    }
    
    if (freedSpace < neededSpace) {
      throw new QuotaExceededError(
        `Input too large: requested ${requiredSpace} tokens, but only ${this._inputQuota - this._inputUsage} available`,
        requiredSpace,
        this._inputQuota - this._inputUsage
      );
    }
    
    this._inputUsage -= freedSpace;
    this.dispatchEvent(new Event('quotaoverflow'));
  }
}

export class LanguageModelImpl implements LanguageModelStatic {
  async create(options: LanguageModelCreateOptions = {}): Promise<LanguageModelSession> {
    if (options.signal?.aborted) {
      throw new DOMException('Operation was aborted', 'AbortError');
    }

    // Handle download monitoring
    if (options.monitor) {
      const monitor = new LanguageModelMonitorImpl();
      options.monitor(monitor);
    }

    // Validate temperature and topK
    if (options.temperature !== undefined && options.temperature < 0) {
      throw new RangeError('Temperature must be >= 0');
    }
    
    if (options.topK !== undefined && options.topK < 1) {
      throw new RangeError('TopK must be >= 1');
    }

    // Apply max limits (these would come from params() in a real implementation)
    const maxTemperature = 2.0;
    const maxTopK = 1000;
    
    if (options.temperature !== undefined && options.temperature > maxTemperature) {
      options.temperature = maxTemperature;
    }
    
    if (options.topK !== undefined && options.topK > maxTopK) {
      options.topK = maxTopK;
    }
    
    if (options.topK !== undefined) {
      options.topK = Math.floor(options.topK);
    }

    return new LanguageModelSessionImpl(options);
  }

  async availability(options: LanguageModelAvailabilityOptions = {}): Promise<LanguageModelAvailability> {
    // In a real implementation, this would check if the model is available,
    // needs downloading, etc. For now, we'll return "available" since we're
    // using the existing threepio infrastructure
    return "available";
  }

  async params(): Promise<LanguageModelParams | null> {
    // Return sensible defaults based on common language model parameters
    return {
      defaultTemperature: 1.0,
      maxTemperature: 2.0,
      defaultTopK: 40,
      maxTopK: 1000,
    };
  }
}

// Create the singleton instance
export const LanguageModel = new LanguageModelImpl();