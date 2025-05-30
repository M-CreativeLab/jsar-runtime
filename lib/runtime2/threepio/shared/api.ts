import type { ChatCompletionCreateParamsBase } from "openai/resources/chat/completions";

export type CustomChatCompletionParams = Omit<ChatCompletionCreateParamsBase, 'messages' | 'model'>;
export type ApiProvider = | 'qwen' | 'doubao';
export type ApiConfiguration = ApiHandlerOptions & {
  apiProvider?: ApiProvider;
};

export interface ApiHandlerOptions {
  apiModelId?: string;
  qwenApiKey?: string;
  doubaoApiKey?: string;
  qwenApiLine?: string;
}

// Qwen
// https://bailian.console.aliyun.com/
export type MainlandQwenModelId = keyof typeof mainlandQwenModels;
export type InternationalQwenModelId = keyof typeof internationalQwenModels;
export const internationalQwenDefaultModelId: InternationalQwenModelId = 'qwen-plus-latest';
export const mainlandQwenDefaultModelId: MainlandQwenModelId = 'qwen-plus-latest';

export const internationalQwenModels = {
  'qwen3-235b-a22b': {
    max_completion_tokens: 129_024,
  },

  'qwen-plus-latest': {
    max_completion_tokens: 129_024,

  }
} as const satisfies Record<string, CustomChatCompletionParams>;

export const mainlandQwenModels = {
  'qwen-plus-latest': {
    max_completion_tokens: 129_024,
    

  }
} as const satisfies Record<string, CustomChatCompletionParams>;

// Doubao
// https://www.volcengine.com/docs/82379/1298459
// https://console.volcengine.com/ark/region:ark+cn-beijing/openManagement
export type DoubaoModelId = keyof typeof doubaoModels;
export const doubaoDefaultModelId: DoubaoModelId = 'doubao-1-5-pro-256k-250115';
export const doubaoModels = {
  'doubao-1-5-pro-256k-250115': {
    max_completion_tokens: 12_288,
  },
  'doubao-1-5-pro-32k-250115': {
    max_completion_tokens: 12_288,

  }
} as const satisfies Record<string, CustomChatCompletionParams>;

