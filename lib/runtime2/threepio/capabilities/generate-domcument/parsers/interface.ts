import { ApiStreamTextChunk } from "../../../api/transform/stream";

export interface Parser {
  parseTextChunk(chunk: ApiStreamTextChunk): void;
  endStream(): void;
}
