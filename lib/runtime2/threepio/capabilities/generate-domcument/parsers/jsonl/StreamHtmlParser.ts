import { ApiStreamTextChunk } from '../../../../api/transform/stream';
import { EmitData, MoudleParserEventType, FragmentType, HtmlFragment, CssFragment } from '../../interfaces';
import { CssOutputFragment, FRAGMENT_FIELD, HTMLOutputFragment } from '../../prompts/jsonl/worker.prompt';
import { Parser } from '../interface';

export interface HtmlStreamItem {
  eventType: MoudleParserEventType;
  data: EmitData | null;
  error?: Error;
}

export class StreamHtmlParser implements Parser {
  readonly #taskId: string;
  #isEnded = false;
  #processedCount = 0;
  #pendingChunks: string[] = [];
  #queue: HtmlStreamItem[] = [];
  #resolve: (() => void) | null = null;

  constructor(taskId: string) {
    this.#taskId = taskId;
  }

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
      eventType: 'streamEnd',
      data: null
    });
    this.#processQueue();
  }

  public async* stream(): AsyncGenerator<HtmlStreamItem, void, unknown> {
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
          this.#resolve = resolve;
          setTimeout(() => resolve(), 10);
        });
        this.#resolve = null;
      }
    }
  }

  #processQueue(): void {
    if (this.#resolve) {
      this.#resolve();
    }
  }

  #parseJsonLine(line: string): HtmlStreamItem | null {
    try {
      const jsonData = JSON.parse(line);
      if (!jsonData || typeof jsonData !== 'object' || !jsonData.type) {
        return {
          eventType: 'error',
          data: null,
          error: new Error('Invalid JSON structure')
        };
      }

      switch (jsonData.type) {
        case 'htmlNode':
          return this.#processHtmlNode(jsonData as HTMLOutputFragment);
        case 'cssRule':
          return this.#processCssRule(jsonData as CssOutputFragment);
        default:
          return {
            eventType: 'error',
            data: null,
            error: new Error(`Unknown type: ${jsonData.type}`)
          };
      }
    } catch (error) {
      return {
        eventType: 'error',
        data: null,
        error: error as Error
      };
    }
  }

  #processHtmlNode(htmlNode: HTMLOutputFragment): HtmlStreamItem | null {
    const parentId = htmlNode[FRAGMENT_FIELD.PARENT_ID];
    const element = htmlNode[FRAGMENT_FIELD.HTML];

    if (!element || !element.trim()) {
      return null;
    }
    const fragment: HtmlFragment = {
      parentId: parentId || null,
      content: element
    };
    return {
      eventType: 'append',
      data: {
        type: FragmentType.HTML,
        fragment
      }
    };
  }

  #processCssRule(cssRule: CssOutputFragment): HtmlStreamItem | null {
    const rule = cssRule[FRAGMENT_FIELD.CSSTEXT];

    if (!rule || !rule.trim()) {
      return null;
    }
    const fragment: CssFragment = {
      content: rule
    };
    return {
      eventType: 'append',
      data: {
        type: FragmentType.CSS,
        fragment
      }
    };
  }
}
