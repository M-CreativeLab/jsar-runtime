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

const appendHtml = (htmlstr: string, element: Element) => {
  const tempDiv = document.createElement('div');
  tempDiv.innerHTML = htmlstr;
  if (element) {
    Array.from(tempDiv.childNodes).forEach(childNode => {
      element.appendChild(childNode);
    });
  } else {
    console.error('Agent: Error appending HTML to memory DOM: element is null');
  }
}

class Threepio {
  #initialDom: boolean = false;
  #browsingContext: Transmute.BrowsingContext;
  #document: Document | null = null; // Store the memory DOM document
  #performanceTracker: PerformanceTracer; // Add performance tracker property
  #controller: Controller = null; // Store the controller instance

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#browsingContext = browsingContext;
    this.#performanceTracker = new PerformanceTracer(); // Initialize tracker
    this.#controller = createController(new PerformanceTracer());

  }

  public async request(input: string) {
    // create html dom
    if (!this.#initialDom) {
      this.#document = this.#browsingContext.start(htmlText, 'text/html', 'text');
      this.#initialDom = true;
    }
    this.#performanceTracker.start('totalTask'); // start total task timer

    try {
      this.#displayDocument(); // Set up event listeners for document updates
      await this.#controller.generatePageStream(input);
    } catch (error) {
      console.error('Agent: Error creating task:', error);
    }

    this.#performanceTracker.end('totalTask'); // Stop total task timer
    this.#performanceTracker.report();

    console.log('Agent: Final HTML:', this.#document.body.innerHTML);
    console.log('Agent: Final CSS:', this.#document.head.innerHTML);
    const htmlcontext = `<html><head>${this.#document.head.innerHTML}</head><body>${this.#document.body.innerHTML}</body></html>`
    this.#saveHtmlToFile(htmlcontext, input);
  }

  #displayDocument() {
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
      console.log('Agent: Processed append key:', type, '  data:', fragment);
    })
  }

  #handleAppendCss(content: string): void {
    console.log('Agent: Processed append CSS:', content);
    if (this.#document) {
      try {
        const styleElement = this.#document.createElement('style');
        styleElement.appendChild(this.#document.createTextNode(content));
        const head = this.#document.head || this.#document.getElementsByTagName('head')[0];
        if (head) {
          head.appendChild(styleElement);
          console.log('Agent: Appended CSS to memory DOM.');
        } else {
          const body = this.#document.body || this.#document.getElementsByTagName('body')[0];
          if (body) {
            body.appendChild(styleElement);
            console.log('Agent: Appended CSS to body in memory DOM (no head found).');
          } else {
            console.warn('Agent: Cannot find head or body to append CSS in memory DOM.');
          }
        }
      } catch (e) {
        console.error('Agent: Error appending CSS to memory DOM:', e);
      }
    }
  }

  #handleAppendHtml(parentid: string, content: string): void {
    console.log('Agent: Process append HTML:', parentid, content);
    if (this.#document) {
      try {
        const document = this.#document;
        if (parentid === null || parentid === 'body') {
          appendHtml(content, document.body);
        }
        else {
          let parentElement = document.getElementById(parentid);
          if (!parentElement) {
            const newParentElement = parentElement = document.createElement('div');
            newParentElement.id = parentid;
            const bodyElement = document.querySelector('body');
            if (bodyElement) {
              bodyElement.appendChild(newParentElement);
            }
            console.log('Agent: Created new parent element:', parentid, content);
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
        console.log('Agent: Appended HTML to memory DOM.');
      } catch (e) {
        console.error('Agent: Error appending HTML to memory DOM:', e);
      }
    }
  }

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
