import { buildApiHandler, LlmMessageParam } from '../api'
import { ApiConfiguration } from '../shared/api'
import { ApiStream } from '../api/transform/stream';
import { getApiKey, getApiModelId, getApiProvider } from './env';

export function callLLM(input, systemPrompt): ApiStream {
  const apiKey = getApiKey();
  const apiProvider = getApiProvider();
  const apiModelId = getApiModelId();
  const config: ApiConfiguration = {
    apiProvider,
    apiModelId,
    qwenApiLine: 'china'
  }
  if (apiProvider === 'doubao') {
    config.doubaoApiKey = apiKey
  } else if (apiProvider === 'qwen') {
    config.qwenApiKey = apiKey
  } else {
    console.error('Unsupported API provider:', apiProvider);
    return null;
  }
  const handler = buildApiHandler(config)
  const messages: LlmMessageParam[] = [{ role: 'user', content: input }];
  return handler.createMessage(systemPrompt, messages)
}
