export interface JsonObject {
  [key: string]: any;
}

export interface ProcessedJsonLine {
  rawLine: string;
  jsonContent?: JsonObject;
  error?: { message: string; code: string };
}

export class JSONLProcessor {
  #buffer: string = '';
  #lineStartIndex: number = 0;

  public *processChunk(chunk: string): Generator<ProcessedJsonLine, void, unknown> {
    if (chunk === null) return;

    this.#buffer += chunk;
    let newlineIndex;

    while ((newlineIndex = this.#buffer.indexOf('\n', this.#lineStartIndex)) !== -1) {
      const line = this.#buffer.substring(this.#lineStartIndex, newlineIndex);
      this.#lineStartIndex = newlineIndex + 1;

      const result = this.#parseLine(line);
      if (result) {
        yield result;
      }
    }

    if (this.#lineStartIndex > 1024) {
      this.#buffer = this.#buffer.substring(this.#lineStartIndex);
      this.#lineStartIndex = 0;
    }
  }

  public *flush(): Generator<ProcessedJsonLine, void, unknown> {
    if (this.#buffer.length > this.#lineStartIndex) {
      const remainingLine = this.#buffer.substring(this.#lineStartIndex);
      const result = this.#parseLine(remainingLine);
      if (result) {
        yield result;
      }
    }
    this.#buffer = '';
    this.#lineStartIndex = 0;
  }

  #parseLine(line: string): ProcessedJsonLine | null {
    const trimmedLine = line.trim();
    if (!trimmedLine) return null;
    try {
      // validate json object
      return {
        rawLine: trimmedLine,
        jsonContent: JSON.parse(trimmedLine) as JsonObject,
      };
    } catch (error) {
      return {
        rawLine: trimmedLine,
        error: {
          message: `JSON parsing error for line: "${trimmedLine}". Error: ${(error as Error).message}`,
          code: 'JSON_PARSE_ERROR'
        }
      };
    }
  }
}
