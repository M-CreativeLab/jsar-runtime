import OpenAI from 'openai';
import type { Anthropic } from '@anthropic-ai/sdk';
import { ApiHandler } from '..';
import {
  ApiHandlerOptions,
  CustomChatCompletionParams,
  doubaoDefaultModelId,
  DoubaoModelId,
  doubaoModels
} from '../../shared/api';
import { convertToOpenAiMessages } from '../transform/openaiFormat';
import { ApiStream } from '../transform/stream';
import { getEndpoint } from '../../utils/env';

export class DoubaoHandler implements ApiHandler {
  #options: ApiHandlerOptions;
  #client: OpenAI;

  constructor(options: ApiHandlerOptions) {
    this.#options = options;
    const baseURL = getEndpoint();
    this.#client = new OpenAI({
      baseURL,
      apiKey: this.#options.doubaoApiKey,
    });
  }

  getModel(): { id: DoubaoModelId; info: CustomChatCompletionParams } {
    const modelId = this.#options.apiModelId;
    if (modelId && modelId in doubaoModels) {
      const id = modelId as DoubaoModelId;
      return { id, info: doubaoModels[id] };
    }
    return {
      id: doubaoDefaultModelId,
      info: doubaoModels[doubaoDefaultModelId],
    };
  }

  async *createMessage(systemPrompt: string, messages: Anthropic.Messages.MessageParam[]): ApiStream {
    const model = this.getModel();
    let openAiMessages: OpenAI.Chat.ChatCompletionMessageParam[] = [
      { role: 'system', content: systemPrompt },
      ...convertToOpenAiMessages(messages),
    ];
    const stream = await this.#client.chat.completions.create({
      model: model.id,
      max_completion_tokens: model.info.max_completion_tokens,
      messages: openAiMessages,
      stream: true,
      stream_options: { include_usage: true },
      temperature: 0,
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
