import { buildApiHandler, LlmMessageParam } from '../api'
import { ApiConfiguration } from '../shared/api'
import { ApiStream } from '../api/transform/stream';
import { getApiKey, getApiModelId, getApiProvider, getApiEndpoint } from '@transmute/env';
import { ApiProvider } from '../shared/api';

export function callLLM(input, systemPrompt): ApiStream {
  const apiKey = getApiKey();
  const apiProvider = getApiProvider() as ApiProvider;
  const apiModelId = getApiModelId();
  const endpoint = getApiEndpoint();
  const config: ApiConfiguration = {
    apiProvider,
    apiModelId,
    apiKey,
    endpoint,
  }
  const handler = buildApiHandler(config)
  const messages: LlmMessageParam[] = [{ role: 'user', content: input }];
  return handler.createMessage(systemPrompt, messages)
}
