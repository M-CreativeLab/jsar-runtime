import { Anthropic } from '@anthropic-ai/sdk';
import { ApiConfiguration, ModelInfo } from '../shared/api';
import { ApiStream, ApiStreamUsageChunk } from './transform/stream';
import { DoubaoHandler } from './providers/doubao';
import { QwenHandler } from './providers/qwen';

/**
 * The API handler is responsible for creating a message and getting the model.
 * It also provides an optional method to get the API stream usage.
 *
 * @interface ApiHandler
 * @property {string} systemPrompt - The system prompt to be used for the message.
 * @property {Anthropic.Messages.MessageParam[]} messages - The messages to be used for the message.
 * @property {() => ApiStream} createMessage - The method to create a message.
 * @property {() => { id: string; info: ModelInfo }} getModel - The method to get the model.
 * @property {() => Promise<ApiStreamUsageChunk | undefined>} getApiStreamUsage - The method to get the API stream usage.
 * @returns {ApiHandler} The API handler.
 */
export interface ApiHandler {
  createMessage(systemPrompt: string, messages: Anthropic.Messages.MessageParam[]): ApiStream
  getModel(): { id: string; info: ModelInfo }
  getApiStreamUsage?(): Promise<ApiStreamUsageChunk | undefined>
}

export function buildApiHandler(configuration: ApiConfiguration): ApiHandler {
  const { apiProvider, ...options } = configuration
  switch (apiProvider) {
    case 'doubao':
      return new DoubaoHandler(options)
    case 'qwen':
      return new QwenHandler(options)
    default:
      throw new Error(`Unsupported API provider: ${apiProvider}`)
  }
}
