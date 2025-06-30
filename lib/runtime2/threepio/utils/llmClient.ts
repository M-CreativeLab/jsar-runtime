import { buildApiHandler } from '../api';
import { ApiConfiguration } from '../shared/api';
import { ApiProvider } from '../shared/api';
import {
  getThreepioApiKey,
  getThreepioApiProvider,
  getThreepioApiModelId,
  getThreepioApiEndpoint,
  isDebugging,
} from '../../../bindings/env';
import { JSONLProcessor } from './JSONLProcessor';
import { reportThreepioError, reportThreepioInfo } from './threepioLog';
import { ApiStream, ApiStreamErrorChunk, ApiStreamJSONChunk } from '../api/transform/stream';

interface CallLLMOptions {
  input: string,
  systemPrompt: string,
}

export function callLLM(options: CallLLMOptions): { stream: ApiStream, requestId: string } {
  const { input, systemPrompt } = options;
  const requestId = input.substring(0, 10) + Date.now();
  const config: ApiConfiguration = {
    apiProvider: getThreepioApiProvider() as ApiProvider,
    apiModelId: getThreepioApiModelId(),
    apiKey: getThreepioApiKey(),
    endpoint: getThreepioApiEndpoint(),
  };
  const handler = buildApiHandler(config);
  try {
    const originalStream = handler.createMessage(systemPrompt, [{ role: 'user', content: input }]);
    const processedStream = processJsonlStream(originalStream, requestId);
    return { stream: processedStream, requestId }; // Return the callId for downstream use
  } catch (error: any) {
    reportThreepioError(error.message, requestId);
  }
}

async function* processJsonlStream(
  sourceStream: ApiStream,
  requestId: string
): ApiStream {
  const jsonlProcessor = new JSONLProcessor();
  try {
    for await (const sourceChunk of sourceStream) {
      if (isDebugging) {
        reportThreepioInfo('Source chunk:', { chunk: sourceChunk, requestId });
      }
      if (sourceChunk.type === 'text') {
        for (const processedLine of jsonlProcessor.processChunk(sourceChunk.text)) {

          if (processedLine.error) {
            const errorMessage = processedLine.error.message;
            reportThreepioError(errorMessage, requestId);
            yield {
              type: 'error',
              error: processedLine.error
            } as ApiStreamErrorChunk;
          } else {
            yield {
              type: 'json',
              jsonObject: processedLine.jsonContent
            } as ApiStreamJSONChunk;
          }
        }
      } else if (sourceChunk.type === 'usage') {
        yield sourceChunk;
      }
    }

    for (const processedLine of jsonlProcessor.flush()) {
      if (processedLine.error) {
        const errorMessage = processedLine.error.message;
        reportThreepioError(errorMessage, requestId);
        yield {
          type: 'error',
          error: processedLine.error
        } as ApiStreamErrorChunk;
      } else {
        yield {
          type: 'json',
          jsonObject: processedLine.jsonContent
        } as ApiStreamJSONChunk;
      }
    }
  } catch (error: any) {
    const errorMessage = `Critical error in LLM stream processing: ${error.message}`;
    reportThreepioError(errorMessage, requestId);
    yield { type: 'error', error: { message: errorMessage, code: 'STREAM_PROCESSING_ERROR' } };
  }
}
