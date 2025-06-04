import type { ChatCompletionCreateParamsBase } from "openai/resources/chat/completions";

export type MoudleInfo = Omit<ChatCompletionCreateParamsBase, 'messages' | 'model'>;

export type ApiProvider = | 'qwen' | 'doubao';

export interface ApiHandlerOptions {
  apiModelId: string;
  apiKey: string;
  endpoint: string;
}

export type ApiConfiguration = ApiHandlerOptions & {
  apiProvider?: ApiProvider;
};

export type LLMModelId = keyof typeof LLMModelToParameters;

export const LLMModelToParameters = {
  'doubao-1-5-pro-256k-250115': {
    max_completion_tokens: 12_288,
  },
  'doubao-1-5-pro-32k-250115': {
    max_completion_tokens: 12_288,
  },
  'qwen3-235b-a22b': {
    max_completion_tokens: 129_024,
  },
  'qwen-plus-latest': {
    max_completion_tokens: 129_024,
  }
} as const satisfies Record<string, MoudleInfo>;
