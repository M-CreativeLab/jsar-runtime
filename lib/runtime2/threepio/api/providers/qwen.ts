import OpenAI from 'openai';
import { ApiHandler, LlmMessageParam } from '..'
import {
  ApiHandlerOptions,
  mainlandQwenModels,
  mainlandQwenDefaultModelId,
  QwenModelId,
  MoudleInfo,
} from '../../shared/api';
import { convertToOpenAiMessages } from '../transform/openaiFormat';
import { ApiStream } from '../transform/stream';
import { getEndpoint } from '@transmute/env';

export class QwenHandler implements ApiHandler {
  #options: ApiHandlerOptions
  #client: OpenAI

  constructor(options: ApiHandlerOptions) {
    this.#options = options
    this.#client = new OpenAI({
      baseURL: getEndpoint(),
      apiKey: this.#options.apiKey,
    })
  }

  getModel(): { id: QwenModelId; info: MoudleInfo } {
    const modelId = this.#options.apiModelId
    // Branch based on API line to let poor typescript know what to do
    return {
      id: (modelId as QwenModelId) ?? mainlandQwenDefaultModelId,
      info: mainlandQwenModels[modelId as QwenModelId] ?? mainlandQwenModels[mainlandQwenDefaultModelId],
    }
  }

  async *createMessage(systemPrompt: string, messages: LlmMessageParam[]): ApiStream {
    const model = this.getModel();
    const openAiMessages: OpenAI.Chat.ChatCompletionMessageParam[] = [
      { role: 'system', content: systemPrompt },
      ...convertToOpenAiMessages(messages),
    ];
    const stream = await this.#client.chat.completions.create({
      model: model.id,
      max_completion_tokens: model.info.max_completion_tokens,
      messages: openAiMessages,
      stream: true,
      stream_options: { include_usage: true },
      ...({ temperature: 0 }),
    });

    for await (const chunk of stream) {
      const delta = chunk.choices[0]?.delta;
      if (delta?.content) {
        yield {
          type: 'text',
          text: delta.content,
        };
      }

      if (chunk.usage) {
        yield {
          type: 'usage',
          inputTokens: chunk.usage.prompt_tokens || 0,
          outputTokens: chunk.usage.completion_tokens || 0,
          // @ts-ignore-next-line
          cacheReadTokens: chunk.usage.prompt_cache_hit_tokens || 0,
          // @ts-ignore-next-line
          cacheWriteTokens: chunk.usage.prompt_cache_miss_tokens || 0,
        };
      }
    }
  }
}
