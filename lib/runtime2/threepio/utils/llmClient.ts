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
import { JsonlStreamProcessor, JsonObject, ProcessedJsonLine } from './JsonlStreamProcessor'; // Assuming jsonlProcessor.ts is in the same directory or path is adjusted
import { reportThreepioError, reportThreepioInfo } from './threepioLog';

interface JsonSegmentTiming {
  startTime: number;
  endTime: number;
  durationMs: number;
  jsonContent: JsonObject; // Added jsonContent
  rawLine: string;
}

interface MonitoredLLMCallData {
  startTime: number;
  endTime: number;
  durationMs: number;
  input: string;
  systemPrompt: string;
  jsonSegments: JsonSegmentTiming[];
  errors?: string[]; // Added errors array
}

export async function* callLLM(input: string, systemPrompt: string): ApiStream {
  const config: ApiConfiguration = {
    apiProvider: getThreepioApiProvider() as ApiProvider,
    apiModelId: getThreepioApiModelId(), // Corrected typo: removed extra 'n'
    apiKey: getThreepioApiKey(),
    endpoint: getThreepioApiEndpoint(),
  };
  const handler = buildApiHandler(config);
  const callStartTime = Date.now();
  const monitoringData: MonitoredLLMCallData = {
    startTime: callStartTime,
    systemPrompt: "systemPrompt",
    input: input,
    jsonSegments: [],
    errors: [],
    endTime: 0,
    durationMs: 0
  };

  const originalStream = handler.createMessage(systemPrompt, [{ role: 'user', content: input }]);
  const processedStream = processJsonlStream(originalStream, monitoringData, '');
  for await (const chunk of processedStream) {
    yield chunk;
  }
}

function updateMonitoringForJsonSegment(
  processedLine: ProcessedJsonLine,
  monitoringData: MonitoredLLMCallData,
  lastSegmentTimeRef: { time: number }
): void {
  const currentTime = Date.now();
  const duration = currentTime - lastSegmentTimeRef.time;
  lastSegmentTimeRef.time = currentTime;

  monitoringData.jsonSegments.push({
    jsonContent: processedLine.jsonContent || processedLine.error || {},
    rawLine: processedLine.rawLine,
    startTime: currentTime - duration,
    endTime: currentTime,
    durationMs: duration,
  });
}

async function* processJsonlStream(
  sourceStream: ApiStream,
  monitoringData: MonitoredLLMCallData,
  spanIdForLogging: string
): ApiStream {
  const jsonlProcessor = new JsonlStreamProcessor();
  const lastSegmentTimeRef = { time: monitoringData.startTime };

  try {
    for await (const sourceChunk of sourceStream) {
      if (sourceChunk.type === 'text') {
        for (const processedLine of jsonlProcessor.processChunk(sourceChunk.text)) {
          if (processedLine.error) {
            const errorMessage = processedLine.error.message;
            reportThreepioError(errorMessage, spanIdForLogging);
            if (!monitoringData.errors) monitoringData.errors = [];
            monitoringData.errors.push(errorMessage);

            yield {
              type: 'error',
              error: processedLine.error
            } as ApiStreamErrorChunk;
            return;
          } else {
            updateMonitoringForJsonSegment(processedLine, monitoringData, lastSegmentTimeRef);
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
        reportThreepioError(errorMessage, spanIdForLogging);
        if (!monitoringData.errors) monitoringData.errors = [];
        monitoringData.errors.push(errorMessage);
        yield {
          type: 'error',
          error: processedLine.error
        } as ApiStreamErrorChunk;
        return;
      } else {
        updateMonitoringForJsonSegment(processedLine, monitoringData, lastSegmentTimeRef);
        yield {
          type: 'text',
          text: processedLine.rawLine
        } as ApiStreamTextChunk;
      }
    }

  } catch (error: any) {
    const errorMessage = `Critical error in LLM stream processing: ${error.message}`;
    reportThreepioError(errorMessage, spanIdForLogging);
    if (!monitoringData.errors) monitoringData.errors = [];
    monitoringData.errors.push(errorMessage);
    yield { type: 'error', error: { message: errorMessage, code: 'STREAM_PROCESSING_ERROR' } };
  } finally {
    monitoringData.endTime = Date.now();
    monitoringData.durationMs = monitoringData.endTime - monitoringData.startTime;
    if (!monitoringData.errors) monitoringData.errors = [];
    reportThreepioInfo(
      'LLM call monitoring data:',
      JSON.stringify(monitoringData, null, 2),
      spanIdForLogging
    );
  }
}
