import { S_HTML_START, S_NODE_START, S_CSS_START, S_HTML_END } from '../separators';
import { ApiStreamTextChunk } from '../../../api/transform/stream';
import { EmitData, MoudleParserEventType, FragmentType, HtmlFragment, StreamHtmlParserCallbacks } from '../interfaces';
import { reportThreepioError, reportThreepioInfo } from '../../../utils/threepioLog';

// Simplified ParseState enum
enum ParseStateSimplified {
  Idle,
  ParsingStream, // Handles all content between S_HTML_START and S_HTML_END
  Finished
}

export class StreamHtmlParser {
  #taskid = '';
  #buffer: string = '';
  #state: ParseStateSimplified = ParseStateSimplified.Idle;
  #callbacks: StreamHtmlParserCallbacks;
  #htmlContent: string = ''; // For debugging or full content retrieval
  #currentStreamType: 'CSS' | 'HTML' | null = null;

  constructor(taskid: string, callbacks: StreamHtmlParserCallbacks) {
    this.#callbacks = callbacks || {};
    this.#taskid = taskid;
  }

  public parseTextChunk(chunk: ApiStreamTextChunk): void {
    if (chunk.type === 'text') {
      this.#buffer += chunk.text;
      this.#htmlContent += chunk.text;
      this.#processBuffer();
    }
  }

  public endStream(): void {
    this.#log('endStream called. Current buffer:', this.#buffer, 'Current state:', ParseStateSimplified[this.#state]);
    // Process any remaining buffer content
    if (this.#buffer.trim().length > 0) {
      reportThreepioError('Stream ended with remaining buffer content:', this.#buffer);
    }
    if (this.#state !== ParseStateSimplified.Finished) {
      this.#log('Finalizing stream state due to endStream call. Current state:', ParseStateSimplified[this.#state]);
      this.#state = ParseStateSimplified.Finished;
      this.#emitStreamEnd();
      this.#log('Stream forcibly ended and finalized by endStream.');
    } else {
      this.#log('Stream already in Finished state during endStream call.');
    }
  }

  #processBuffer(isLastChunk: boolean = false): void {
    this.#log('Processing buffer:', this.#buffer, 'isLastChunk:', isLastChunk);
    if (!this.#buffer) {
      return;
    }
    let lines = this.#buffer.split('\n');
    const isBufferComplete = isLastChunk || this.#buffer.endsWith('\n') || this.#buffer.endsWith(S_HTML_END);
    const linesToProcess = isBufferComplete ? lines : lines.slice(0, -1);
    this.#buffer = isBufferComplete ? '' : lines[lines.length - 1];
    for (const line of linesToProcess) {
      const trimmed = line.trim();
      if (trimmed) this.#parseLine(trimmed);
    }
  }

  #emitHtmlFragment(parentId: string | null, htmlElement: string): void {
    if (htmlElement && htmlElement.trim()) {
      this.#emitDataFun('append', {
        type: FragmentType.HTML,
        fragment: { parentId, content: htmlElement } as HtmlFragment
      });
      this.#log('Emitted HTML fragment:', { parentId, htmlElement });
    }
  }

  #emitCssFragment(rule: string): void {
    if (rule && rule.trim()) {
      this.#emitDataFun('append', {
        type: FragmentType.CSS,
        fragment: { content: rule }
      });
      this.#log('Emitted CSS fragment:', { rule });
    }
  }

  #changeStreamType(newType: 'CSS' | 'HTML' | null): void {
    if (this.#currentStreamType === newType) {
      return;
    }
    this.#currentStreamType = newType;
  }

  #emitStreamEnd(): void {
    this.#changeStreamType(null);
    if (this.#callbacks && typeof this.#callbacks.onStreamEnd === 'function') {
      this.#callbacks.onStreamEnd();
    }
    this.#emitDataFun('streamEnd', null);
    this.#log('Emitted streamEnd. Full content received:', this.#htmlContent);
  }

  #parseLine(line: string): void {
    switch (this.#state) {
      case ParseStateSimplified.Idle:
        if (line === S_HTML_START) {
          this.#state = ParseStateSimplified.ParsingStream;
        } else {
          this.#log('Warning: Expected S_HTML_START in Idle state, got:', line);
        }
        break;
      case ParseStateSimplified.ParsingStream:
        if (line.startsWith(S_CSS_START)) {
          this.#changeStreamType('CSS');
          const cssRuleContent = line.substring(S_CSS_START.length);
          this.#emitCssFragment(cssRuleContent);
        } else if (line.startsWith(S_NODE_START)) {
          this.#changeStreamType('HTML');
          const content = line.substring(S_NODE_START.length);
          const separatorIndex = content.indexOf(':');
          if (separatorIndex > -1) {
            const parentIdStr = content.substring(0, separatorIndex);
            const htmlElement = content.substring(separatorIndex + 1);
            const parentId = parentIdStr === 'NULL_PARENT' || parentIdStr === 'null' ? null : parentIdStr;
            this.#emitHtmlFragment(parentId, htmlElement);
          } else {
            this.#log('Warning: Malformed S_NODE_START line (no separator):', line);
          }
        } else if (line === S_HTML_END) {
          this.#state = ParseStateSimplified.Finished;
          this.#emitStreamEnd();
        } else {
          if (this.#currentStreamType === 'CSS') {
            this.#emitCssFragment(line);
          } else {
            this.#log('Warning: Unknown line in ParsingStream state:', line);
          }
        }
        break;
      case ParseStateSimplified.Finished:
        if (line.trim() !== '') {
          this.#log('Warning: Received data after stream finished:', line);
        }
        break;
    }
  }

  #log(...msg: any[]): void {
    // For debugging, uncomment the line below
    reportThreepioInfo('StreamParser taskid:', this.#taskid, ...msg);
  }

  #emitDataFun(eventType: MoudleParserEventType, data: EmitData): void {
    if (this.#callbacks && typeof this.#callbacks.onData === 'function') {
      this.#callbacks.onData(eventType, data);
    }
  }
}
