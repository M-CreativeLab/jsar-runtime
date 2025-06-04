import { APP_ROOT_ID } from '.';
import { EmitData } from './interfaces';

function appendHtml(htmlstr: string, element: Element) {
  const tempDiv = document.createElement('div');
  tempDiv.innerHTML = htmlstr;
  Array.from(tempDiv.childNodes).forEach(childNode => {
    element.appendChild(childNode);
  });
}

export class DomOperator {
  public operate(document: Document, data: EmitData): void {
    if (document === null) {
      console.warn('Agent: Document is null, cannot operate on it.');
      return;
    }
    const { type, fragment } = data;
    const { id, content, parentId } = fragment as any;;
    switch (type) {
      case 'html':
        this.#appendFragment(document, parentId, content);
        break;
      case 'css':
        this.#appendCss(document, content);
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
        this.#appendFragment(document, APP_ROOT_ID, moudleHtmlfragment);
        console.log('Agent: Processed moudle append CSS:', moudleCssfragment);
        this.#appendCss(document, moudleCssfragment);
        break;
      default:
        console.warn('Agent: Processed append key:', type, ' data:', fragment);
        break;
    }
  }

  /**
   * 
   * @param content  css content to be appended.
   * @description This method appends the provided CSS content to the document's head element.
   */
  #appendCss(document: Document, content: string): void {
    console.log('Agent: Processed append CSS:', content);
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

  /**
   * 
   * @param selectId  The ID of the parent element to which the HTML content will be appended.
   * @param content  The HTML content to be appended.
   * @description This method appends the provided HTML content to the document's body element.
   */
  #appendFragment(document: Document, selectId: string, content: string): void {
    console.log('Agent: Process append HTML:', selectId, content);
    try {
      if (selectId === null || selectId === 'body') {
        appendHtml(content, document.body);
      } else {
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
