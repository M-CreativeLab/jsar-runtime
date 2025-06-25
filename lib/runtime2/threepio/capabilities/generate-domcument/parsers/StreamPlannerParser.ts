import { Parser, PlannerStreamItem } from './interface';
import { ApiStreamChunk } from '../../../api/transform/stream';
import { ParsedHeader, ParsedModule, ParsedPlannerFields } from '../interfaces';
import { JsonObject } from '../../../utils/JSONLProcessor';

export class StreamPlannerParser implements Parser {
  #isEnded = false;
  #processedCount = 0;
  #resolve: ((value: PlannerStreamItem | null) => void) | null = null;
  #queue: PlannerStreamItem[] = [];

  public parseChunk(chunk: ApiStreamChunk): void {
    if (this.#isEnded || chunk.type !== 'json' || !chunk.jsonObject) {
      return;
    }

    const item = this.#transformObject(chunk.jsonObject);
    if (item) {
      this.#processedCount++;
      this.#queue.push(item);
    }
    this.#processQueue();
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
    while (true) {
      while (this.#queue.length > 0) {
        yield this.#queue.shift()!;
      }

      if (this.#isEnded && this.#queue.length === 0) {
        break;
      }

      await new Promise<void>(resolve => {
        this.#resolve = () => resolve();
      });
      this.#resolve = null;
    }
  }

  #processQueue(): void {
    if (this.#resolve) {
      this.#resolve(null);
    }
  }

  #transformObject(data: JsonObject): PlannerStreamItem | null {
    try {
      if (!data || typeof data !== 'object' || !data.type) {
        return null;
      }
      switch (data.type) {
        case 'planHeader':
          return this.#transformPlanHeader(data);
        case 'planModule':
          return this.#transformPlanModule(data);
        default:
          return {
            type: 'error',
            data: { error: new Error(`Unknown type: ${data.type}`), content: JSON.stringify(data) }
          };
      }
    } catch (error) {
      return {
        type: 'error',
        data: { error: error as Error, content: JSON.stringify(data) }
      };
    }
  }

  #transformPlanHeader(data: JsonObject): PlannerStreamItem | null {
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

  #transformPlanModule(data: JsonObject): PlannerStreamItem | null {
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
