import { buildApiHandler } from '../api'
import { ApiConfiguration } from '../shared/api'
import { ApiStream } from '../api/transform/stream';
import { getApiKey, getApiModelId, getApiProvider, getApiEndpoint } from '@transmute/env';
import { ApiProvider } from '../shared/api';

export function callLLM(input, systemPrompt): ApiStream {
  const config: ApiConfiguration = {
    apiProvider: getApiProvider() as ApiProvider,
    apiModelId: getApiModelId(),
    apiKey: getApiKey(),
    endpoint: getApiEndpoint(),
  };
  const handler = buildApiHandler(config);
  return handler.createMessage(systemPrompt, [{ role: 'user', content: input }])
}
