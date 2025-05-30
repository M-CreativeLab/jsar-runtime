
import fs from 'fs';
import { APP_ROOT_ID } from '.';

function appendHtml(htmlstr: string, element: Element) {
  const tempDiv = document.createElement('div');
  tempDiv.innerHTML = htmlstr;
  Array.from(tempDiv.childNodes).forEach(childNode => {
    element.appendChild(childNode);
  });
}

export class Operator {

  public operate(document: Document, data) {
    // note: testing background color change
    // const randomColor = '#' + Math.floor(Math.random() * 16777215).toString(16);
    // this.#document.body.style.backgroundColor = 'rgba(255, 255, 255,0.2)';
    const { type, fragment } = data;
    const { id, content, parentId } = fragment;
    console.log('Agent: Processed append key:', type, 'data:', fragment);
    switch (type) {
      case 'html':
        this.#appendHtml(document, parentId, content);
        break;
      case 'css':
        this.#appendCss(document, fragment);
        break;
      case 'header':
        if (!content) {
          console.warn('Agent: Processed append key:', type, 'data:', fragment);
          return;
        }
        const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
        console.log('Agent: Processed  Header append CSS:', headerCssfragment);
        this.#appendCss(document, headerCssfragment);
        break;
      case 'moudle':
        const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
        const moudleCssfragment = `#${id}{${content}}`;
        this.#appendHtml(document, APP_ROOT_ID, moudleHtmlfragment);
        console.log('Agent: Processed moudle append CSS:', moudleCssfragment);
        this.#appendCss(document, moudleCssfragment);
        break;
      default:
        console.warn('Agent: Processed append key:', type, ' data:', fragment);
        break;
    }
  }

  /**
 * Saves the generated HTML content to a file.
 * @param html - The HTML content to be saved.
 * @param input - The input string used to generate the HTML content.
 * @description This method creates a timestamped filename based on the input string,
 * and saves the HTML content to a file in the specified directory.
 */
  public saveHtmlToFile(html: string, input: string): void {
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

  /**
   * 
   * @param content  css content to be appended.
   * @description This method appends the provided CSS content to the document's head element.
   */
  #appendCss(document: Document, content: string): void {
    console.log('Agent: Processed append CSS:', content);
    if (document) {
      try {
        const styleElement = document.createElement('style');
        styleElement.appendChild(document.createTextNode(content));
        const head = document.head;
        if (head) {
          head.appendChild(styleElement);
        } else {
          const body = document.body || document.getElementsByTagName('body')[0];
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
  #appendHtml(document: Document, selectId: string, content: string): void {
    console.log('Agent: Process append HTML:', selectId, content);
    if (document) {
      try {
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
}
