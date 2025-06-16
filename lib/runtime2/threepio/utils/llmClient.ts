import { buildApiHandler } from '../api';
import { ApiConfiguration } from '../shared/api';
import { ApiStream, ApiStreamErrorChunk, ApiStreamTextChunk } from '../api/transform/stream';
import { ApiProvider } from '../shared/api';
import {
  getThreepioApiKey,
  getThreepioApiProvider,
  getThreepioApiModelId,
  getThreepioApiEndpoint,
} from '../../../bindings/env';
import { JsonlStreamProcessor } from './jsonlProcessor';
import { reportThreepioError } from './threepioLog';
import { LLMMonitoringService } from './LLMMonitoringService';

const monitoringService = LLMMonitoringService.getInstance();

export async function callLLM(
  input: string,
  systemPrompt: string,
  parentId?: string | null
): Promise<{ stream: ApiStream; callId: string }> {
  const config: ApiConfiguration = {
    apiProvider: getThreepioApiProvider() as ApiProvider,
    apiModelId: getThreepioApiModelId(),
    apiKey: getThreepioApiKey(),
    endpoint: getThreepioApiEndpoint(),
  };
  const handler = buildApiHandler(config);

  const callId = monitoringService.startCall(input, parentId || undefined);
  monitoringService.recordSystemPrompt(callId, systemPrompt);
  monitoringService.recordInput(callId, input);

  try {
    const originalStream = handler.createMessage(systemPrompt, [{ role: 'user', content: input }]);
    const processedStream = processJsonlStream(originalStream, callId);
    return { stream: processedStream, callId };
  } catch (error: any) {
    monitoringService.recordError(callId, error.message || String(error));
    throw error;
  }
}

async function* processJsonlStream(
  sourceStream: ApiStream,
  callId: string // Pass callId for monitoring
): ApiStream {
  const jsonlProcessor = new JsonlStreamProcessor();
  try {
    for await (const sourceChunk of sourceStream) {
      if (sourceChunk.type === 'text') {
        for (const processedLine of jsonlProcessor.processChunk(sourceChunk.text)) {
          if (processedLine.error) {
            const errorMessage = processedLine.error.message;
            monitoringService.recordError(callId, `JSONL parsing error: ${errorMessage}`);
            reportThreepioError(errorMessage, callId);
            yield {
              type: 'error',
              error: processedLine.error
            } as ApiStreamErrorChunk;
          } else if (processedLine.jsonContent) {
            yield {
              type: 'text',
              text: processedLine.rawLine
            } as ApiStreamTextChunk;
            const endTime = Date.now();
            monitoringService.addJsonOutputTiming(callId, {
              endTime,
              jsonContent: processedLine.jsonContent,
              rawLine: processedLine.rawLine
            });
          } else {
            yield {
              type: 'text',
              text: processedLine.rawLine
            } as ApiStreamTextChunk;
          }
        }
      } else if (sourceChunk.type === 'usage') {
        yield sourceChunk;
      }
    }

    for (const processedLine of jsonlProcessor.flush()) {
      if (processedLine.error) {
        const errorMessage = processedLine.error.message;
        monitoringService.recordError(callId, `JSONL flushing error: ${errorMessage}`);
        reportThreepioError(errorMessage, callId);
        yield {
          type: 'error',
          error: processedLine.error
        } as ApiStreamErrorChunk;
      } else if (processedLine.jsonContent) {
        yield {
          type: 'text',
          text: processedLine.rawLine
        } as ApiStreamTextChunk;
        const endTime = Date.now();
        monitoringService.addJsonOutputTiming(callId, {
          endTime,
          jsonContent: processedLine.jsonContent,
          rawLine: processedLine.rawLine
        });
      }
    }
  } catch (error: any) {
    const errorMessage = `Critical error in LLM stream processing: ${error.message}`;
    monitoringService.recordError(callId, errorMessage);
    reportThreepioError(errorMessage, callId);
    yield { type: 'error', error: { message: errorMessage, code: 'STREAM_PROCESSING_ERROR' } };
  } finally {
    monitoringService.endCall(callId);
  }
}
