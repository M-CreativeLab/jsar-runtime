
import { ask } from '../utils/llmUtils';
import { ApiStream } from '../api/transform/stream';
import { getApiKey, getApiProvider, getApiModelId } from '../utils/envUtils';

export function callLLM(input: string, prompt: string): ApiStream {
  const apiKey = getApiKey();
  const apiProvider = getApiProvider();
  const apiModelId = getApiModelId();
  return ask({ input, systemPrompt: prompt, apiProvider, apiKey, apiModelId });
}

