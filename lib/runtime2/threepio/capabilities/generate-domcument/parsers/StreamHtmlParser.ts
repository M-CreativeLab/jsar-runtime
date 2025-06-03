import { S_HTML_START, S_NODE_START, S_CSS_START, S_HTML_END } from '../separators';
import { ApiStreamChunk } from '../../../api/transform/stream';
import { EmitterEventType, FragmentType, StreamHtmlParserCallbacks } from '../interfaces';

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

  public parseChunk(chunk: ApiStreamChunk): void {
    if (chunk.type === 'text') {
      this.#buffer += chunk.text;
      this.#htmlContent += chunk.text;
      this.#processBuffer();
    } else if (chunk.type === 'usage') {
      if (this.#buffer.length > 0) {
        this.#processBuffer(true);
      }
      this.endStream();
    }
  }

  #processBuffer(isLastChunk: boolean = false): void {
    let lines = this.#buffer.split('\n');
    if (!isLastChunk && lines.length <= 1 && !this.#buffer.endsWith(S_HTML_END)) {
      return;
    }

    const linesToProcess = (!isLastChunk && lines.length > 1) ? lines.slice(0, -1) : lines;
    this.#buffer = (!isLastChunk && lines.length > 1) ? lines[lines.length - 1] : '';

    for (const line of linesToProcess) {
      if (line.trim() === '') continue;
      this.#parseLine(line.trim());
    }
  }

  #emitHtmlFragment(parentId: string | null, htmlElement: string): void {
    this.#emitDataFun(EmitterEventType.append, {
      type: FragmentType.HTML,
      fragment: { parentId, content: htmlElement }
    });
    this.#log('Emitted HTML fragment:', { parentId, htmlElement });
  }

  #emitCssFragment(rule: string): void {
    this.#emitDataFun(EmitterEventType.append, {
      type: FragmentType.CSS,
      fragment: rule
    });
    this.#log('Emitted CSS fragment:', { rule });
  }

  #changeStreamType(newType: 'CSS' | 'HTML' | null): void {
    if (this.#currentStreamType === newType) return;
    this.#currentStreamType = newType;
  }

  #emitStreamEnd(): void {
    // Ensure any active sub-stream is closed before the main stream ends
    this.#changeStreamType(null); // This will close any open CSS or HTML stream
    this.#emitDataFun('streamEnd', null);
    this.#log('Emitted streamEnd. Full content received:', this.#htmlContent);
  }

  #parseLine(line: string): void {
    // this.#log(`Parsing line: '${line}' in state: ${ParseStateSimplified[this.#state]}`);
    switch (this.#state) {
      case ParseStateSimplified.Idle:
        if (line === S_HTML_START) {
          this.#state = ParseStateSimplified.ParsingStream;
          // As per prompt, first block is CSS. #changeStreamType will handle cssStreamStart.
        } else {
          this.#log('Warning: Expected S_HTML_START in Idle state, got:', line);
        }
        break;
      case ParseStateSimplified.ParsingStream:
        if (line.startsWith(S_CSS_START)) {
          this.#changeStreamType('CSS');
          const cssRuleContent = line.substring(S_CSS_START.length);
          if (cssRuleContent.trim().length > 0) {
            this.#emitCssFragment(cssRuleContent.trim());
          }
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
          this.#emitStreamEnd(); // This will also close any open CSS/HTML sub-stream
        } else {
          // Handle lines that are part of multi-line CSS rules or unexpected content
          if (this.#currentStreamType === 'CSS') {
            // Assuming multi-line CSS rules don't have S_CSS_START on each line
            this.#emitCssFragment(line.trim());
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

  public endStream(): void {
    this.#log('endStream called. Current buffer:', this.#buffer, 'Current state:', ParseStateSimplified[this.#state]);
    if (this.#buffer.length > 0) {
      this.#log('endStream processing remaining buffer as last chunk.');
      const finalLines = this.#buffer.split('\n');
      this.#buffer = '';
      for (const line of finalLines) {
        if (line.trim() === '') continue;
        this.#log('endStream parsing final line:', line);
        this.#parseLine(line.trim());
      }
    }

    if (this.#state !== ParseStateSimplified.Finished) {
      this.#log('Finalizing stream state due to endStream call. Current state:', ParseStateSimplified[this.#state]);
      this.#state = ParseStateSimplified.Finished;
      this.#emitStreamEnd(); // This will also close any open CSS/HTML sub-stream
      this.#log('Stream forcibly ended and finalized by endStream.');
    } else {
      this.#log('Stream already in Finished state during endStream call.');
    }
  }

  #log(...msg: any[]): void {
    // For debugging, uncomment the line below
    console.log('StreamParser taskid:', this.#taskid, ...msg);
  }
  #emitDataFun(eventType: string, data: any): void {
    if (this.#callbacks && typeof this.#callbacks.onData === 'function') {
      this.#callbacks.onData(eventType, data);
    }
  }
}
