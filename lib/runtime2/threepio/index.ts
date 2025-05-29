import fs from 'fs';
import { ApiProvider } from './shared/api';
import { PerformanceTracer } from './utils/PerformanceTracer';
import { ApiStream } from './api/transform/stream';
import { create } from './planner/Controller';

const APP_ROOT_ID = 'jsar-app-root';
const htmlText = `
<html>
  <head></head>
  <body style="display:flex; justify-content:center; align-items:center; background-color:#fff;">
    <div id=\"${APP_ROOT_ID}\">
    </div>
  </body>
</html>`;
class threepio {
  initialDom: boolean = false;

  browsingContext: Transmute.BrowsingContext;
  memoryDocument: Document | null = null; // Store the memory DOM document
  performanceTracker: PerformanceTracer; // Add performance tracker property

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.browsingContext = browsingContext;
    this.performanceTracker = new PerformanceTracer(); // Initialize tracker
  }

  public async creatTask(input: string) {
    // create html dom
    if (!this.initialDom) {
      this.memoryDocument = this.browsingContext.start(htmlText, 'text/html');
      this.initialDom = true;
    }
    this.performanceTracker.start('totalTask'); // start total task timer

    const controller = create(new PerformanceTracer());
    controller.on('append', (data) => {
      // const randomColor = '#' + Math.floor(Math.random() * 16777215).toString(16);
      // this.memoryDocument.body.style.backgroundColor = 'rgba(255, 255, 255,0.2)';
      const { type, fragment } = data;
      const { id, content, parentId } = fragment;
      console.log('Agent: Processed append key:', type, 'data:', fragment);
      switch (type) {
        case 'html':
          this.handleAppendHtml(parentId, content);
          break;
        case 'css':
          this.handleAppendCss(fragment);
          break;
        case 'header':
          if (!content) {
            console.warn('Agent: Processed append key:', type, 'data:', fragment);
            return;
          }
          const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
          console.log('Agent: Processed  Header append CSS:', headerCssfragment);
          this.handleAppendCss(headerCssfragment);
          break;
        case 'moudle':
          const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
          const moudleCssfragment = `#${id}{${content}}`;
          this.handleAppendHtml(APP_ROOT_ID, moudleHtmlfragment);
          console.log('Agent: Processed moudle append CSS:', moudleCssfragment);
          this.handleAppendCss(moudleCssfragment);
          break;
        default:
          console.warn('Agent: Processed append key:', type, ' data:', fragment);
          break;
      }
      console.log('Agent: Processed append key:', type, '  data:', fragment);
    })

    try {
      await controller.generatePageStream(input);
    } catch (error) {
      console.error('Agent: Error creating task:', error);
    }

    this.performanceTracker.end('totalTask'); // Stop total task timer
    this.performanceTracker.report();

    console.log('Agent: Final HTML:', this.memoryDocument.body.innerHTML);
    console.log('Agent: Final CSS:', this.memoryDocument.head.innerHTML);
    const htmlcontext = `<html><head>${this.memoryDocument.head.innerHTML}</head><body>${this.memoryDocument.body.innerHTML}</body></html>`
    this.saveHtmlToFile(htmlcontext, input);
    return controller;

  }

  private handleAppendCss(content: string) {
    console.log('Agent: Processed append CSS:', content);
    if (this.memoryDocument) {
      try {
        const styleElement = this.memoryDocument.createElement('style');
        styleElement.appendChild(this.memoryDocument.createTextNode(content));
        const head = this.memoryDocument.head || this.memoryDocument.getElementsByTagName('head')[0];
        if (head) {
          head.appendChild(styleElement);
          console.log('Agent: Appended CSS to memory DOM.');
        } else {
          const body = this.memoryDocument.body || this.memoryDocument.getElementsByTagName('body')[0];
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

  private handleAppendHtml(parentid: string, content: string) {
    console.log('Agent: Process append HTML:', parentid, content);
    if (this.memoryDocument) {
      try {
        const document = this.memoryDocument;
        if (parentid === null || parentid === 'body') {
          this.appendHtml(content, document.body);
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
          this.appendHtml(content, parentElement);
        }
        console.log('Agent: Appended HTML to memory DOM.');
      } catch (e) {
        console.error('Agent: Error appending HTML to memory DOM:', e);
      }
    }
  }


  private appendHtml(htmlstr: string, element: Element) {
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

  private saveHtmlToFile(html: string, input: string) {
    const timestamp = new Date().getTime();
    const sanitizedInput = input;//.substring(0, 50);
    const folderName = `./.cache/tmp/`;
    if (!fs.existsSync(folderName)) {
      fs.mkdirSync(folderName);
    }
    const name = `${sanitizedInput}_${timestamp}.html`;
    const filename = `${folderName}${name}`;
    console.log(`Agent: HTML saved to ${filename}`);
    fs.writeFileSync(filename, html);
  }
}
export {
  ApiProvider,
  threepio,
  ApiStream
};