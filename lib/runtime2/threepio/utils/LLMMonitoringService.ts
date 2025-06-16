import { JsonObject } from './jsonlProcessor';

interface JsonSegmentTiming {
  startTime: number;
  endTime: number;
  durationMs: number;
  jsonContent: JsonObject;
  rawLine: string;
}

// Interface for the data passed to addJsonOutputTiming by the client
interface IncomingJsonSegmentData {
  endTime: number;
  jsonContent: JsonObject;
  rawLine: string;
}

export interface MonitoredLLMCallData {
  callId: string;
  parentId?: string;
  prompt?: string;
  startTime: number;
  endTime: number;
  inputTokens?: number;
  outputTokens?: number;
  error?: string;
  jsonOutputTimings: JsonSegmentTiming[];
  childCalls?: MonitoredLLMCallData[];
}

export class LLMMonitoringService {
  static instance: LLMMonitoringService;
  #activeCalls: Map<string, MonitoredLLMCallData> = new Map();
  #monitoredRootCallIds: MonitoredLLMCallData;

  public static getInstance(): LLMMonitoringService {
    if (!LLMMonitoringService.instance) {
      LLMMonitoringService.instance = new LLMMonitoringService();
    }
    return LLMMonitoringService.instance;
  }

  startCall(input: string, parentId?: string): string {

    const callId = `${input}-${Date.now()}`;
    const callData: MonitoredLLMCallData = {
      callId,
      parentId,
      endTime: 0,
      startTime: Date.now(),
      jsonOutputTimings: [],
    };
    this.#activeCalls.set(callId, callData);

    // If a parentId is provided, add this call to the parent's childCalls list
    if (parentId) {
      const parentCall = this.#activeCalls.get(parentId);
      if (parentCall) {
        if (!parentCall.childCalls) {
          parentCall.childCalls = [];
        }
        parentCall.childCalls.push(callData);
      }
    } else {
      this.#monitoredRootCallIds = callData;
    }
    return callId;
  }

  recordInput(callId: string, prompt: string, inputTokens?: number) {
    const call = this.#activeCalls.get(callId);
    if (call) {
      call.prompt = prompt;
      call.inputTokens = inputTokens;
    }
  }

  recordSystemPrompt(callId: string, systemPrompt: string) {
    const call = this.#activeCalls.get(callId);
    if (call) {
      call.prompt = call.prompt ? `${systemPrompt}\n\n${call.prompt}` : systemPrompt;
    }
  }

  addJsonOutputTiming(callId: string, incomingSegment: IncomingJsonSegmentData) {
    const call = this.#activeCalls.get(callId);
    if (call) {
      if (!call.jsonOutputTimings) {
        call.jsonOutputTimings = [];
      }

      let startTime: number;
      if (call.jsonOutputTimings.length === 0) {
        // This is the first segment for this call
        startTime = call.startTime; // Use the call's start time
      } else {
        // Subsequent segments use the previous segment's end time
        startTime = call.jsonOutputTimings[call.jsonOutputTimings.length - 1].endTime;
      }

      const durationMs = incomingSegment.endTime - startTime;

      const fullSegment: JsonSegmentTiming = {
        ...incomingSegment,
        startTime,
        durationMs,
      };

      call.jsonOutputTimings.push(fullSegment);
    }
  }

  recordError(callId: string, error: string) {
    const call = this.#activeCalls.get(callId);
    if (call) {
      call.error = error;
      call.endTime = Date.now();
      console.error(`[LLM Call Error] ID: ${callId}, Error: ${error}`);
    }
  }

  endCall(callId: string) { // Removed outputTokens parameter
    const call = this.#activeCalls.get(callId);
    if (call && !call.endTime) { // Ensure not already ended (e.g., by error)
      call.endTime = Date.now();
      const duration = call.endTime - call.startTime;
      console.log(
        `[LLM Call End] ID: ${callId}, Duration: ${duration}ms` // Removed Output Tokens from log
      );
    }
  }

  getAllCallData(structured: boolean = false): MonitoredLLMCallData[] {
    const allCalls = Array.from(this.#activeCalls.values());
    if (!structured) {
      return allCalls; // Return flat list as before
    }

    const rootCalls = allCalls.filter(call => !call.parentId || !this.#activeCalls.has(call.parentId));

    return rootCalls;
  }

  printAll() {
    console.log('All LLM Calls:', JSON.stringify(this.#monitoredRootCallIds, null, 2));
  }
}
