import { ApiStreamTextChunk } from "../../../api/transform/stream";
import { EmitData, MoudleParserEventType, ParsedHeader, ParsedModule } from "../interfaces";

/**
 * Stream for generating HTML fragments.
 */
export type HtmlGenerateStream = AsyncGenerator<HtmlStreamItem, void, unknown>;

/**
 * Stream for generating planner items.
 */
export type PlannerGenerateStream = AsyncGenerator<PlannerStreamItem, void, unknown>;

/**
 * Parser interface for processing text chunks from the API stream.
 */
export interface Parser {
  parseTextChunk(chunk: ApiStreamTextChunk): void;
  endStream(): void;
}

/**
 *  define planner stream item
 */
export interface PlannerStreamItem {
  type: 'header' | 'module' | 'end' | 'error';
  data?: ParsedHeader | ParsedModule | { processedCount: number } | { error: Error; content: string };
}

/**
 * define html stream item
 */
export interface HtmlStreamItem {
  eventType: MoudleParserEventType;
  data: EmitData | null;
  error?: Error;
}
