import { Anthropic } from '@anthropic-ai/sdk';
import { ApiConfiguration, ModelInfo } from '../shared/api';
import { ApiStream, ApiStreamUsageChunk } from './transform/stream';
import { DoubaoHandler } from './providers/doubao';
import { QwenHandler } from './providers/qwen';

/**
 * Interface for handling API calls to large language models, returning streaming data.
 *
 * This interface defines the contract for creating messages, retrieving model information,
 * and optionally obtaining API stream usage statistics.
 */
export interface ApiHandler {
  //`createMessage` is used to send a system prompt and a list of messages to the model, returning a streaming response.
  createMessage(systemPrompt: string, messages: Anthropic.Messages.MessageParam[]): ApiStream
  // retrieves the model's identifier and related information.
  getModel(): { id: string; info: ModelInfo }
  // (optional) provides usage statistics for the API stream, if available.
  getApiStreamUsage?(): Promise<ApiStreamUsageChunk | undefined>
}

export function buildApiHandler(configuration: ApiConfiguration): ApiHandler {
  const { apiProvider, ...options } = configuration
  switch (apiProvider) {
    case 'doubao':
      return new DoubaoHandler(options);
    case 'qwen':
      return new QwenHandler(options);
    default:
      throw new Error(`Unsupported API provider: ${apiProvider}`);
  }
}
