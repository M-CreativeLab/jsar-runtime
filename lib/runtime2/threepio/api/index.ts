import type { Anthropic } from '@anthropic-ai/sdk';
import { ApiConfiguration, MoudleInfo } from '../shared/api';
import { ApiStream, ApiStreamUsageChunk } from './transform/stream';
import { DoubaoHandler } from './providers/doubao';
import { QwenHandler } from './providers/qwen';

export type LLMMessageParam = Anthropic.Messages.MessageParam;

/**
 * Interface for defining APIs to a specific large language model on cloud.
 * @interface ApiHandler
 * @returns A promise that resolves to an `ApiStreamUsageChunk` object containing usage data, or `undefined` if not available.
 */
export interface ApiHandler {
  //`createMessage` is used to send a system prompt and a list of messages to the model, returning a streaming response.
  createMessage(systemPrompt: string, messages: LLMMessageParam[]): ApiStream;
  // retrieves the model's identifier and related information.
  getModel(): { id: string; info: MoudleInfo };
  // (optional) provides usage statistics for the API stream, if available.
  getApiStreamUsage?(): Promise<ApiStreamUsageChunk | undefined>;
}

export function buildApiHandler(configuration: ApiConfiguration): ApiHandler {
  const { apiProvider, ...options } = configuration;
  switch (apiProvider) {
    case 'doubao':
      return new DoubaoHandler(options);
    case 'qwen':
      return new QwenHandler(options);
    default:
      throw new Error(`Unsupported API provider: ${apiProvider}`);
  }
}
