import { ParsedHeader, ParsedModule, ParsedPlannerFields } from '../../interfaces';
import { ApiStreamTextChunk } from '../../../../api/transform/stream';
import { Parser } from '../interface';

export interface PlannerStreamItem {
  type: 'header' | 'module' | 'end' | 'error';
  data?: ParsedHeader | ParsedModule | { processedCount: number } | { error: Error; content: string };
}

export class StreamPlannerParser implements Parser {
  #isEnded = false;
  #processedCount = 0;
  #pendingChunks: string[] = [];
  #resolve: ((value: PlannerStreamItem | null) => void) | null = null;
  #queue: PlannerStreamItem[] = [];

  public parseTextChunk(chunk: ApiStreamTextChunk): void {
    if (this.#isEnded || chunk.type !== 'text' || !chunk.text) {
      return;
    }

    const trimmedLine = chunk.text.trim();
    if (trimmedLine) {
      this.#pendingChunks.push(trimmedLine);
      this.#processQueue();
    }
  }

  public endStream(): void {
    if (this.#isEnded) return;

    this.#isEnded = true;
    this.#queue.push({
      type: 'end',
      data: { processedCount: this.#processedCount }
    });
    this.#processQueue();
  }

  public async* stream(): AsyncGenerator<PlannerStreamItem, void, unknown> {
    while (!this.#isEnded || this.#queue.length > 0 || this.#pendingChunks.length > 0) {
      while (this.#pendingChunks.length > 0) {
        const chunk = this.#pendingChunks.shift()!;
        const item = this.#parseJsonLine(chunk);
        if (item) {
          this.#processedCount++;
          yield item;
        }
      }

      while (this.#queue.length > 0) {
        yield this.#queue.shift()!;
      }

      if (!this.#isEnded && this.#queue.length === 0 && this.#pendingChunks.length === 0) {
        await new Promise<void>(resolve => {
          this.#resolve = () => resolve();
          setTimeout(() => resolve(), 10); // 防止死锁
        });
        this.#resolve = null;
      }
    }
  }

  #processQueue(): void {
    if (this.#resolve) {
      this.#resolve(null);
    }
  }

  #parseJsonLine(jsonString: string): PlannerStreamItem | null {
    try {
      const data = JSON.parse(jsonString);
      if (!data || typeof data !== 'object' || !data.type) {
        return null;
      }
      switch (data.type) {
        case 'planHeader':
          return this.#processPlanHeader(data);
        case 'planModule':
          return this.#processPlanModule(data);
        default:
          return {
            type: 'error',
            data: { error: new Error(`Unknown type: ${data.type}`), content: jsonString }
          };
      }
    } catch (error) {
      return {
        type: 'error',
        data: { error: error as Error, content: jsonString }
      };
    }
  }

  #processPlanHeader(data: any): PlannerStreamItem | null {
    const requiredFields = [ParsedPlannerFields.name, ParsedPlannerFields.theme, ParsedPlannerFields.layout];
    const missingFields = requiredFields.filter(field => !data[field]);

    if (missingFields.length > 0) {
      return {
        type: 'error',
        data: { error: new Error(`Missing fields: ${missingFields.join(', ')}`), content: JSON.stringify(data) }
      };
    }

    const header: ParsedHeader = {
      appName: data[ParsedPlannerFields.name],
      overallTheme: data[ParsedPlannerFields.theme],
      layout: data[ParsedPlannerFields.layout],
    };

    return { type: 'header', data: header };
  }

  #processPlanModule(data: any): PlannerStreamItem | null {
    const requiredFields = [ParsedPlannerFields.name, ParsedPlannerFields.layout, ParsedPlannerFields.description];
    const missingFields = requiredFields.filter(field => !data[field]);

    if (missingFields.length > 0) {
      return {
        type: 'error',
        data: { error: new Error(`Missing fields: ${missingFields.join(', ')}`), content: JSON.stringify(data) }
      };
    }

    const module: ParsedModule = {
      name: data[ParsedPlannerFields.name],
      layout: data[ParsedPlannerFields.layout],
      description: data[ParsedPlannerFields.description],
    };

    return { type: 'module', data: module };
  }
}
