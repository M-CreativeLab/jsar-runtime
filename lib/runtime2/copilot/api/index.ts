import { Anthropic } from '@anthropic-ai/sdk'
import { ApiConfiguration, ModelInfo } from '../shared/api'
import { ApiStream, ApiStreamUsageChunk } from './transform/stream'
import { DoubaoHandler } from './providers/doubao'
import { QwenHandler } from './providers/qwen'


export interface ApiHandler {
	createMessage(systemPrompt: string, messages: Anthropic.Messages.MessageParam[]): ApiStream
	getModel(): { id: string; info: ModelInfo }
	getApiStreamUsage?(): Promise<ApiStreamUsageChunk | undefined>
}

export interface SingleCompletionHandler {
	completePrompt(prompt: string): Promise<string>
}

export function buildApiHandler(configuration: ApiConfiguration): ApiHandler {
	const { apiProvider, ...options } = configuration
	switch (apiProvider) {
		case 'doubao':
			return new DoubaoHandler(options)
		case 'qwen':
			return new QwenHandler(options)
		default:
			return new QwenHandler(options)
	}
}
