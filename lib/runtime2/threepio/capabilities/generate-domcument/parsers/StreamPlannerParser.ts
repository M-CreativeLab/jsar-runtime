import { EventEmitter } from 'events';
import { PLANNER_HEADER_MARKER, PLANNER_MODULE_MARKER, PLANNER_END_MARKER } from '../separators';
import { ParsedHeader, ParsedModule, PlanParserEventType } from '../interfaces';
import { ParsedPlannerFields } from '../interfaces'
import { ApiStreamChunk } from '../../../api/transform/stream';
import { threepioError, threepioLog, threepioWarn } from '../../../utils/threepioLog';

enum PlannerParseState {
  WaitingForHeader,
  ProcessingModules,
  Finished
}

// Helper to find the end of a JSON object (simple version, assumes no nested {} in strings)
function findJsonEnd(buffer: string, startIndex: number): number {
  if (startIndex === -1 || buffer[startIndex] !== '{') return -1;
  let braceCount = 0;
  for (let i = startIndex; i < buffer.length; i++) {
    if (buffer[i] === '{') braceCount++;
    else if (buffer[i] === '}') braceCount--;
    if (braceCount === 0) return i;
  }
  return -1; // Closing brace not found
}

function findNextMarker(buffer: string, startIndex: number): number {
  const nextModuleIdx = buffer.indexOf(PLANNER_MODULE_MARKER, startIndex);
  const endIdx = buffer.indexOf(PLANNER_END_MARKER, startIndex);
  if (nextModuleIdx !== -1 && endIdx !== -1) {
    return Math.min(nextModuleIdx, endIdx);
  } else if (nextModuleIdx !== -1) {
    return nextModuleIdx;
  } else if (endIdx !== -1) {
    return endIdx;
  }
  return -1; // No known marker found
}

export interface StreamPlannerParser {
  on(event: PlanParserEventType, listener: (data: any) => void): this;
  emit(event: PlanParserEventType, data?: any): boolean;
}

export class StreamPlannerParser extends EventEmitter {
  #buffer: string = '';
  #currentModule: string = '';
  #state: PlannerParseState = PlannerParseState.WaitingForHeader;

  public parseChunk(chunk: ApiStreamChunk) {
    if (chunk.type === 'text') {
      this.#buffer += chunk.text;
      this.#currentModule += chunk.text;
      this.#processBuffer();
    }
  }

  public end() {
    this.#processBuffer(true);
    if (this.#state === PlannerParseState.Finished) {
      this.#emitData('parseEnd', this.#currentModule);
    }
  }

  #processBuffer(finalPass: boolean = false) {
    let processedSomethingInThisIteration = true;

    while (processedSomethingInThisIteration && this.#state !== PlannerParseState.Finished) {
      processedSomethingInThisIteration = false;

      let nextMarkerIndex = -1;
      let contentToParse = '';
      let markerFound = '';
      let expectedMarker = '';

      switch (this.#state) {
        case PlannerParseState.WaitingForHeader:
          expectedMarker = PLANNER_HEADER_MARKER;
          if (this.#buffer.startsWith(expectedMarker)) {
            nextMarkerIndex = findNextMarker(this.#buffer, expectedMarker.length);
            if (nextMarkerIndex !== -1) {
              // Content is between the opening brace after marker and the next marker
              const jsonStart = this.#buffer.indexOf('{', expectedMarker.length);
              const jsonEnd = findJsonEnd(this.#buffer, jsonStart);
              if (jsonStart !== -1 && jsonEnd !== -1 && jsonEnd < nextMarkerIndex) {
                contentToParse = this.#buffer.substring(jsonStart, jsonEnd + 1);
                markerFound = this.#buffer.substring(nextMarkerIndex, nextMarkerIndex + PLANNER_MODULE_MARKER.length); // Assuming next is M: or E:
              } else if (finalPass && jsonStart !== -1 && jsonEnd === -1) {
                // Incomplete JSON at the end of stream
                threepioWarn('[processBuffer] Incomplete JSON for header at end of stream.');
              }
            }
          } else if (finalPass && this.#buffer.trim().length > 0) {
            threepioError('Malformed start: Expected H: marker.');
            this.#state = PlannerParseState.Finished; // Avoid infinite loop
            return;
          }
          break;

        case PlannerParseState.ProcessingModules:
          expectedMarker = PLANNER_MODULE_MARKER;
          if (this.#buffer.startsWith(expectedMarker)) {
            nextMarkerIndex = findNextMarker(this.#buffer, expectedMarker.length);
            if (nextMarkerIndex !== -1) {
              const jsonStart = this.#buffer.indexOf('{', expectedMarker.length);
              const jsonEnd = findJsonEnd(this.#buffer, jsonStart);
              if (jsonStart !== -1 && jsonEnd !== -1 && jsonEnd < nextMarkerIndex) {
                contentToParse = this.#buffer.substring(jsonStart, jsonEnd + 1);
                markerFound = this.#buffer.substring(nextMarkerIndex, nextMarkerIndex + PLANNER_MODULE_MARKER.length); // Could be M: or E:
              } else if (finalPass && jsonStart !== -1 && jsonEnd === -1) {
                threepioWarn('[processBuffer] Incomplete JSON for module at end of stream.');
              }
            }
          } else if (this.#buffer.startsWith(PLANNER_END_MARKER)) {
            // This case is handled by the final check or if E: is found as nextMarker
            nextMarkerIndex = 0; // Process E: marker
            markerFound = PLANNER_END_MARKER;
          } else if (finalPass && this.#buffer.trim().length > 0) {
            threepioError('Malformed module content or missing M: or E: marker.');
            this.#state = PlannerParseState.Finished; // Avoid infinite loop
            return;
          }
          break;
      }

      if (contentToParse) {
        if (this.#tryParseJsonContent(contentToParse)) {
          // Successfully parsed, advance buffer past the parsed JSON and its preceding marker
          const consumedLength = this.#buffer.indexOf(contentToParse) + contentToParse.length;
          this.#buffer = this.#buffer.substring(consumedLength).trimStart(); // Trim leading whitespace before next marker
          // The next marker (M: or E:) is now at the beginning of the buffer if findNextMarker this.#buffer,worked correctly
          // Or we need to re-evaluate based on the new buffer state.
          // For simplicity, let's assume findNextMarker this.#buffer,correctly positions us for the next state update.
          // The updateState logic needs to be robust based on what's now at the start of the buffer.
          this.#updateStateBasedOnBuffer();
          processedSomethingInThisIteration = true;
        } else {
          // JSON parsing failed, wait for more data unless final pass
          if (finalPass) threepioError('Malformed JSON content detected:', contentToParse);
        }
      } else if (nextMarkerIndex !== -1 && markerFound === PLANNER_END_MARKER) {
        // Handle E: marker directly if it's the next significant thing
        this.#buffer = this.#buffer.substring(this.#buffer.indexOf(PLANNER_END_MARKER) + PLANNER_END_MARKER.length).trimStart();
        this.#state = PlannerParseState.Finished;
        processedSomethingInThisIteration = true;
      } else if (finalPass && this.#buffer.startsWith(PLANNER_END_MARKER)) {
        this.#buffer = this.#buffer.substring(PLANNER_END_MARKER.length).trimStart();
        this.#state = PlannerParseState.Finished;
        processedSomethingInThisIteration = true;
      }
    }
  }

  #tryParseJsonContent(jsonString: string): boolean {
    try {
      const data = JSON.parse(jsonString);
      if (this.#state === PlannerParseState.WaitingForHeader) {
        if (data[ParsedPlannerFields.name] && data[ParsedPlannerFields.theme] && data[ParsedPlannerFields.layout]) {
          const header: ParsedHeader = {
            appName: data[ParsedPlannerFields.name],
            overallTheme: data[ParsedPlannerFields.theme],
            layout: data[ParsedPlannerFields.layout] as ParsedHeader['layout'],
          };
          threepioLog('Parsed header (JSON):', header);
          this.#emitData('headerParsed', header);
          return true;
        } else {
          threepioWarn('[tryParseJsonContent] Invalid header JSON structure:', jsonString, 'data:', data);
        }
      } else if (this.#state === PlannerParseState.ProcessingModules) {
        if (data[ParsedPlannerFields.name] && data[ParsedPlannerFields.layout] && data[ParsedPlannerFields.description]) {
          const module: ParsedModule = {
            name: data[ParsedPlannerFields.name],
            layout: data[ParsedPlannerFields.layout] as ParsedModule['layout'],
            description: data[ParsedPlannerFields.description],
          };
          this.#emitData('moduleParsed', module);
          return true;
        } else {
          threepioWarn('[tryParseJsonContent] Invalid module JSON structure:', data);
        }
      }
    } catch (e) {
      threepioError('[tryParseJsonContent] JSON parsing error:', e, 'Content:', jsonString);
    }
    return false;
  }

  #emitData(event: PlanParserEventType, content?: any) {
    this.emit(event, content);
  }

  #updateStateBasedOnBuffer() {
    if (this.#buffer.startsWith(PLANNER_MODULE_MARKER)) {
      this.#state = PlannerParseState.ProcessingModules;
    } else if (this.#buffer.startsWith(PLANNER_END_MARKER)) {
      this.#state = PlannerParseState.Finished;
    } else if (this.#buffer.startsWith(PLANNER_HEADER_MARKER) && this.#state !== PlannerParseState.WaitingForHeader) {
      // This case should ideally not happen if flow is correct, but as a fallback
      threepioWarn('[updateStateBasedOnBuffer] Encountered Header marker unexpectedly. Resetting to WaitingForHeader.')
      this.#state = PlannerParseState.WaitingForHeader;
    }
  }
}
