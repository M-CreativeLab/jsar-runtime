import { EventEmitter } from 'events';
import { PLANNER_HEADER_MARKER, PLANNER_MODULE_MARKER, PLANNER_END_MARKER } from '../prompt/promptSeparators';
import { ParsedHeader, ParsedModule } from '../interfaces';
import { ParsedPlannerFields } from '../interfaces'
import { ApiStreamChunk } from '../../api/transform/stream';
enum PlannerParseState {
  WaitingForHeader,
  ProcessingModules,
  Finished
}

export class StreamPlannerParser extends EventEmitter {
  private buffer: string = '';
  private currentModule: string = '';
  private state: PlannerParseState = PlannerParseState.WaitingForHeader;
  private emiter: EventEmitter;

  constructor(emt: EventEmitter) {
    super();
    this.emiter = emt;
  }

  public processChunk(chunk: ApiStreamChunk) {
    if (chunk.type === 'text') {
      this.buffer += chunk.text;
      this.currentModule += chunk.text;
      this.processBuffer();
    }
  }

  public endStream() {
    this.processBuffer(true); // Final pass
    if (this.state !== PlannerParseState.Finished) {
      console.warn("Planner stream ended unexpectedly or with incomplete JSON. Current buffer:", this.buffer);
    }
    this._emit('parseEnd');
    console.log("Planner JSON stream parsing finished.", this.currentModule);
  }

  private processBuffer(finalPass: boolean = false) {
    let processedSomethingInThisIteration = true;

    while (processedSomethingInThisIteration && this.state !== PlannerParseState.Finished) {
      processedSomethingInThisIteration = false;

      let nextMarkerIndex = -1;
      let contentToParse = '';
      let markerFound = '';
      let expectedMarker = '';

      switch (this.state) {
        case PlannerParseState.WaitingForHeader:
          expectedMarker = PLANNER_HEADER_MARKER;
          if (this.buffer.startsWith(expectedMarker)) {
            nextMarkerIndex = this.findNextMarker(expectedMarker.length);
            if (nextMarkerIndex !== -1) {
              // Content is between the opening brace after marker and the next marker
              const jsonStart = this.buffer.indexOf('{', expectedMarker.length);
              const jsonEnd = this.findJsonEnd(jsonStart);
              if (jsonStart !== -1 && jsonEnd !== -1 && jsonEnd < nextMarkerIndex) {
                contentToParse = this.buffer.substring(jsonStart, jsonEnd + 1);
                markerFound = this.buffer.substring(nextMarkerIndex, nextMarkerIndex + PLANNER_MODULE_MARKER.length); // Assuming next is M: or E:
              } else if (finalPass && jsonStart !== -1 && jsonEnd === -1) {
                // Incomplete JSON at the end of stream
                console.warn("[processBuffer] Incomplete JSON for header at end of stream.");
              }
            }
          } else if (finalPass && this.buffer.trim().length > 0) {
            console.error("Malformed start: Expected H: marker.");
            this.state = PlannerParseState.Finished; // Avoid infinite loop
            return;
          }
          break;

        case PlannerParseState.ProcessingModules:
          expectedMarker = PLANNER_MODULE_MARKER;
          if (this.buffer.startsWith(expectedMarker)) {
            nextMarkerIndex = this.findNextMarker(expectedMarker.length);
            if (nextMarkerIndex !== -1) {
              const jsonStart = this.buffer.indexOf('{', expectedMarker.length);
              const jsonEnd = this.findJsonEnd(jsonStart);
              if (jsonStart !== -1 && jsonEnd !== -1 && jsonEnd < nextMarkerIndex) {
                contentToParse = this.buffer.substring(jsonStart, jsonEnd + 1);
                markerFound = this.buffer.substring(nextMarkerIndex, nextMarkerIndex + PLANNER_MODULE_MARKER.length); // Could be M: or E:
              } else if (finalPass && jsonStart !== -1 && jsonEnd === -1) {
                console.warn("[processBuffer] Incomplete JSON for module at end of stream.");
              }
            }
          } else if (this.buffer.startsWith(PLANNER_END_MARKER)) {
            // This case is handled by the final check or if E: is found as nextMarker
            nextMarkerIndex = 0; // Process E: marker
            markerFound = PLANNER_END_MARKER;
          } else if (finalPass && this.buffer.trim().length > 0) {
            console.error("Malformed module content or missing M: or E: marker.");
            this.state = PlannerParseState.Finished; // Avoid infinite loop
            return;
          }
          break;
      }

      if (contentToParse) {
        if (this.tryParseJsonContent(contentToParse)) {
          // Successfully parsed, advance buffer past the parsed JSON and its preceding marker
          const consumedLength = this.buffer.indexOf(contentToParse) + contentToParse.length;
          this.buffer = this.buffer.substring(consumedLength).trimStart(); // Trim leading whitespace before next marker
          // The next marker (M: or E:) is now at the beginning of the buffer if findNextMarker worked correctly
          // Or we need to re-evaluate based on the new buffer state.
          // For simplicity, let's assume findNextMarker correctly positions us for the next state update.
          // The updateState logic needs to be robust based on what's now at the start of the buffer.
          this.updateStateBasedOnBuffer();
          processedSomethingInThisIteration = true;
        } else {
          // JSON parsing failed, wait for more data unless final pass
          if (finalPass) console.error("Malformed JSON content detected:", contentToParse);
        }
      } else if (nextMarkerIndex !== -1 && markerFound === PLANNER_END_MARKER) {
        // Handle E: marker directly if it's the next significant thing
        this.buffer = this.buffer.substring(this.buffer.indexOf(PLANNER_END_MARKER) + PLANNER_END_MARKER.length).trimStart();
        this.state = PlannerParseState.Finished;
        processedSomethingInThisIteration = true;
      } else if (finalPass && this.buffer.startsWith(PLANNER_END_MARKER)) {
        this.buffer = this.buffer.substring(PLANNER_END_MARKER.length).trimStart();
        this.state = PlannerParseState.Finished;
        processedSomethingInThisIteration = true;
      }
    }
  }

  // Helper to find the end of a JSON object (simple version, assumes no nested {} in strings)
  private findJsonEnd(startIndex: number): number {
    if (startIndex === -1 || this.buffer[startIndex] !== '{') return -1;
    let braceCount = 0;
    for (let i = startIndex; i < this.buffer.length; i++) {
      if (this.buffer[i] === '{') braceCount++;
      else if (this.buffer[i] === '}') braceCount--;
      if (braceCount === 0) return i;
    }
    return -1; // Closing brace not found
  }

  private findNextMarker(startIndex: number): number {
    const nextModuleIdx = this.buffer.indexOf(PLANNER_MODULE_MARKER, startIndex);
    const endIdx = this.buffer.indexOf(PLANNER_END_MARKER, startIndex);

    if (nextModuleIdx !== -1 && endIdx !== -1) {
      return Math.min(nextModuleIdx, endIdx);
    } else if (nextModuleIdx !== -1) {
      return nextModuleIdx;
    } else if (endIdx !== -1) {
      return endIdx;
    }
    return -1; // No known marker found
  }

  private tryParseJsonContent(jsonString: string): boolean {
    try {
      const data = JSON.parse(jsonString);
      if (this.state === PlannerParseState.WaitingForHeader) {
        if (data[ParsedPlannerFields.name] && data[ParsedPlannerFields.theme] && data[ParsedPlannerFields.layout]) {
          const header: ParsedHeader = {
            appName: data[ParsedPlannerFields.name],
            overallTheme: data[ParsedPlannerFields.theme],
            layout: data[ParsedPlannerFields.layout] as ParsedHeader['layout'],
          };
          console.log("Parsed header (JSON):", header);
          this._emit('headerParsed', header);
          return true;
        } else {
          console.warn("[tryParseJsonContent] Invalid header JSON structure:", jsonString, "data:", data);
        }
      } else if (this.state === PlannerParseState.ProcessingModules) {
        if (data[ParsedPlannerFields.name] && data[ParsedPlannerFields.layout] && data[ParsedPlannerFields.description]) {
          const module: ParsedModule = {
            name: data[ParsedPlannerFields.name],
            layout: data[ParsedPlannerFields.layout] as ParsedModule['layout'],
            description: data[ParsedPlannerFields.description],
          };
          this._emit('moduleParsed', module);
          return true;
        } else {
          console.warn("[tryParseJsonContent] Invalid module JSON structure:", data);
        }
      }
    } catch (e) {
      console.error("[tryParseJsonContent] JSON parsing error:", e, "Content:", jsonString);
    }
    return false;
  }

  private _emit(event: string, content?: any) {
    this.emiter.emit(event, content);
  }

  private updateStateBasedOnBuffer() {
    if (this.buffer.startsWith(PLANNER_MODULE_MARKER)) {
      this.state = PlannerParseState.ProcessingModules;
    } else if (this.buffer.startsWith(PLANNER_END_MARKER)) {
      this.state = PlannerParseState.Finished;
    } else if (this.buffer.startsWith(PLANNER_HEADER_MARKER) && this.state !== PlannerParseState.WaitingForHeader) {
      // This case should ideally not happen if flow is correct, but as a fallback
      console.warn("[updateStateBasedOnBuffer] Encountered Header marker unexpectedly. Resetting to WaitingForHeader.")
      this.state = PlannerParseState.WaitingForHeader;
    }
    // If no marker is at the start, we wait for more data or it's an error handled in processBuffer
  }
}