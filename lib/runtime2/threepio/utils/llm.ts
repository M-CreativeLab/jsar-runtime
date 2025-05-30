import { buildApiHandler, LlmMessageParam } from '../api'
import { ApiConfiguration, ApiProvider } from '../shared/api'
import { ApiStream } from '../api/transform/stream';

function ask(
  { input,
    systemPrompt,
    apiProvider,
    apiKey,
    apiModelId }:
    {
      input: string,
      systemPrompt: string,
      apiProvider: ApiProvider,
      apiKey: string,
      apiModelId: string
    }
): ApiStream {
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
export { ask };
