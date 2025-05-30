
import { ask } from '../utils/llm';
import { ApiStream } from '../api/transform/stream';
import { getApiKey, getApiProvider, getApiModelId } from '../utils/env';

export function callLLM(input: string, prompt: string): ApiStream {
  const apiKey = getApiKey();
  const apiProvider = getApiProvider();
  const apiModelId = getApiModelId();
  return ask({ input, systemPrompt: prompt, apiProvider, apiKey, apiModelId });
}

