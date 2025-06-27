import { HtmlStreamItem, Parser } from './interface';
import { ApiStreamChunk } from '../../../api/transform/stream';
import { FragmentType, HtmlFragment, CssFragment } from '../interfaces';
import { CssOutputFragment, FRAGMENT_FIELD, HTMLOutputFragment } from '../prompts/worker.prompt';
import { JsonObject } from '../../../utils/JSONLProcessor';

export class StreamHtmlParser implements Parser {
  readonly #taskId: string;
  #isEnded = false;
  #queue: HtmlStreamItem[] = [];
  #resolve: (() => void) | null = null;

  constructor(taskId: string) {
    this.#taskId = taskId;
  }

  public parseChunk(chunk: ApiStreamChunk): void {
    if (this.#isEnded || chunk.type !== 'json' || !chunk.jsonObject) {
      return;
    }

    const item = this.#transformObject(chunk.jsonObject);
    if (item) {
      this.#queue.push(item);
    }
    this.#processQueue();
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
    while (true) {
      while (this.#queue.length > 0) {
        yield this.#queue.shift()!;
      }

      if (this.#isEnded && this.#queue.length === 0) {
        break;
      }

      await new Promise<void>(resolve => {
        this.#resolve = resolve;
      });
      this.#resolve = null;
    }
  }

  #processQueue(): void {
    if (this.#resolve) {
      this.#resolve();
    }
  }

  #transformObject(jsonData: JsonObject): HtmlStreamItem | null {
    try {
      if (!jsonData || typeof jsonData !== 'object' || !jsonData.type) {
        return {
          eventType: 'error',
          data: null,
          error: new Error('Invalid JSON structure')
        };
      }

      switch (jsonData.type) {
        case 'htmlNode':
          return this.#transformHtmlNode(jsonData as HTMLOutputFragment);
        case 'cssRule':
          return this.#transformCssRule(jsonData as CssOutputFragment);
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

  #transformHtmlNode(htmlNode: HTMLOutputFragment): HtmlStreamItem | null {
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
        fragment,
        requestId: '',
      }
    };
  }

  #transformCssRule(cssRule: CssOutputFragment): HtmlStreamItem | null {
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
        fragment,
        requestId: '',
      }
    };
  }
}
