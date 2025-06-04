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

// Qwen
export type QwenModelId = keyof typeof models;
export const qwenDefaultModelId: QwenModelId = 'qwen-plus-latest';
// Doubao
export type DoubaoModelId = keyof typeof models;
export const doubaoDefaultModelId: DoubaoModelId = 'doubao-1-5-pro-256k-250115';

export const models = {
  'doubao-1-5-pro-256k-250115': {
    max_completion_tokens: 12_288,
  },
  'doubao-1-5-pro-32k-250115': {
    max_completion_tokens: 12_288,

  }, 'qwen3-235b-a22b': {
    max_completion_tokens: 129_024,
  },
  'qwen-plus-latest': {
    max_completion_tokens: 129_024,
  }
} as const satisfies Record<string, MoudleInfo>;
