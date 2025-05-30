import fs from 'fs';
import { ApiProvider } from './shared/api';
import { PerformanceTracer } from './utils/PerformanceTracer';
import { ApiStream } from './api/transform/stream';
import { Controller, createController } from './planner/Controller';

const APP_ROOT_ID = 'app-root';

const htmlText = `
<html>
  <head></head>
  <body style="display:flex; justify-content:center; align-items:center; background-color:#fff;">
    <div id=\"${APP_ROOT_ID}\">
    </div>
  </body>
</html>`;

function appendHtml(htmlstr: string, element: Element) {
  const tempDiv = document.createElement('div');
  tempDiv.innerHTML = htmlstr;
  Array.from(tempDiv.childNodes).forEach(childNode => {
    element.appendChild(childNode);
  });
}

class Threepio {
  #browsingContext: Transmute.BrowsingContext;
  #document: Document | null = null;
  #performanceTracker: PerformanceTracer;
  #controller: Controller = null;

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#browsingContext = browsingContext;
    this.#performanceTracker = new PerformanceTracer();
    this.#controller = createController(new PerformanceTracer());
  }

  /**
   * 
   * @param input input string that will be processed to generate HTML content.
   * @returns A Promise that resolves when the HTML content has been generated and displayed in the space.
   * @description This method initializes the HTML document, starts the performance tracker,
   * and calls the controller to generate the HTML content based on the input string.
   * It then stops the performance tracker and reports the performance metrics.
   * Finally, it saves the generated HTML content to a file.
   */
  public async request(input: string): Promise<void> {
    // create html dom
    this.#document = this.#browsingContext.start(htmlText, 'text/html', 'text');
    this.#performanceTracker.start('total'); // start total task timer
    try {
      this.#displayDocument(); // Set up event listeners for document updates
      await this.#controller.generatePageStream(input);
    } catch (error) {
      console.error('Agent: Error creating task:', error);
    }
    this.#performanceTracker.end('total'); // Stop total task timer
    this.#performanceTracker.report();
    console.log('Agent: Final HTML:', this.#document.body.innerHTML);
    console.log('Agent: Final CSS:', this.#document.head.innerHTML);
    const htmlcontext = `<html><head>${this.#document.head.innerHTML}</head><body>${this.#document.body.innerHTML}</body></html>`
    this.#saveHtmlToFile(htmlcontext, input);
  }

  /**
   * Saves the generated HTML content to a file.
   * @param htmlContent - The HTML content to be saved.
   * @param input - The input string used to generate the HTML content.
   * @description This method saves the generated HTML content to a file named after the input string.
   */
  #displayDocument(): void {
    this.#controller.on('append', (data) => {
      // note: testing background color change
      // const randomColor = '#' + Math.floor(Math.random() * 16777215).toString(16);
      // this.#document.body.style.backgroundColor = 'rgba(255, 255, 255,0.2)';
      const { type, fragment } = data;
      const { id, content, parentId } = fragment;
      console.log('Agent: Processed append key:', type, 'data:', fragment);
      switch (type) {
        case 'html':
          this.#handleAppendHtml(parentId, content);
          break;
        case 'css':
          this.#handleAppendCss(fragment);
          break;
        case 'header':
          if (!content) {
            console.warn('Agent: Processed append key:', type, 'data:', fragment);
            return;
          }
          const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
          console.log('Agent: Processed  Header append CSS:', headerCssfragment);
          this.#handleAppendCss(headerCssfragment);
          break;
        case 'moudle':
          const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
          const moudleCssfragment = `#${id}{${content}}`;
          this.#handleAppendHtml(APP_ROOT_ID, moudleHtmlfragment);
          console.log('Agent: Processed moudle append CSS:', moudleCssfragment);
          this.#handleAppendCss(moudleCssfragment);
          break;
        default:
          console.warn('Agent: Processed append key:', type, ' data:', fragment);
          break;
      }
    });
  }

  /**
   * 
   * @param content  css content to be appended.
   * @description This method appends the provided CSS content to the document's head element.
   */
  #handleAppendCss(content: string): void {
    console.log('Agent: Processed append CSS:', content);
    if (this.#document) {
      try {
        const styleElement = this.#document.createElement('style');
        styleElement.appendChild(this.#document.createTextNode(content));
        const head = this.#document.head;
        if (head) {
          head.appendChild(styleElement);
        } else {
          const body = this.#document.body || this.#document.getElementsByTagName('body')[0];
          if (body) {
            body.appendChild(styleElement);
            console.log('Agent: Appended CSS to body in DOM (no head found).');
          } else {
            console.warn('Agent: Cannot find head or body to append CSS in DOM.');
          }
        }
      } catch (e) {
        console.error('Agent: Error appending CSS to DOM:', e);
      }
    }
  }

  /**
   * 
   * @param selectId  The ID of the parent element to which the HTML content will be appended.
   * @param content  The HTML content to be appended.
   * @description This method appends the provided HTML content to the document's body element.
   */
  #handleAppendHtml(selectId: string, content: string): void {
    console.log('Agent: Process append HTML:', selectId, content);
    if (this.#document) {
      try {
        const document = this.#document;
        if (selectId === null || selectId === 'body') {
          appendHtml(content, document.body);
        }
        else {
          let parentElement = document.getElementById(selectId);
          if (!parentElement) {
            const newParentElement = parentElement = document.createElement('div');
            newParentElement.id = selectId;
            const bodyElement = document.body;
            if (bodyElement) {
              bodyElement.appendChild(newParentElement);
            }
            console.log('Agent: Created new parent element:', selectId, content);
          } else {
            const tempDiv = document.createElement('div');
            tempDiv.innerHTML = content;
            if (tempDiv.getAttribute('class')) {
              parentElement.setAttribute('class', tempDiv.getAttribute('class'));
              console.log('Agent: Set attribute:', tempDiv.getAttribute('class'));
            }
            if (tempDiv.getAttribute('style')) {
              parentElement.setAttribute('style', tempDiv.getAttribute('style'));
              console.log('Agent: Set attribute:', tempDiv.getAttribute('style'));
            }
          }
          appendHtml(content, parentElement);
        }
        console.log('Agent: Appended HTML to DOM.');
      } catch (e) {
        console.error('Agent: Error appending HTML to DOM:', e);
      }
    }
  }

  /**
   * Saves the generated HTML content to a file.
   * @param html - The HTML content to be saved.
   * @param input - The input string used to generate the HTML content.
   * @description This method creates a timestamped filename based on the input string,
   * and saves the HTML content to a file in the specified directory.
   */
  #saveHtmlToFile(html: string, input: string): void {
    const timestamp = new Date().getTime();
    const sanitizedInput = input;//.substring(0, 50);
    const folderName = `./.cache/tmp/`;
    if (!fs.existsSync(folderName)) {
      fs.mkdirSync(folderName);
    }
    const name = `${sanitizedInput}_${timestamp}.html`;
    const filename = `${folderName}${name}`;
    console.log(`Agent: HTML saved to ${filename}`);
    fs.promises.writeFile(filename, html)
      .then(() => {
        console.log(`Agent: HTML file written successfully.`);
      })
      .catch((err) => {
        console.error('Agent: Error writing HTML file:', err);
      });
  }
}
export {
  ApiProvider,
  Threepio,
  ApiStream
};
