import { buildApiHandler } from '../api'
import { ApiConfiguration } from '../shared/api'
import { ApiStream } from '../api/transform/stream';
import { ApiProvider } from '../shared/api';
import {
  getThreepioApiKey,
  getThreepioApiEndpoint,
  getThreepioApiModelId,
  getThreepioApiProvider
} from '@transmute/env';

export function callLLM(input, systemPrompt): ApiStream {
  const config: ApiConfiguration = {
    apiProvider: getThreepioApiProvider() as ApiProvider,
    apiModelId: getThreepioApiModelId(),
    apiKey: getThreepioApiKey(),
    endpoint: getThreepioApiEndpoint(),
  };
  const handler = buildApiHandler(config);
  return handler.createMessage(systemPrompt, [{ role: 'user', content: input }])
}
