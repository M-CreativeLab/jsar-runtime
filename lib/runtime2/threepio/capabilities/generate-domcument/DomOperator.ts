import { APP_ROOT_ID } from '.';
import { threepioError, threepioLog, threepioWarn } from '../../utils/threepioLog';
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
      threepioWarn('Document is null, cannot operate on it.');
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
          threepioWarn('Processed append key:', type, 'data:', fragment);
          return;
        }
        const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
        threepioLog('Processed  Header append CSS:', headerCssfragment);
        this.#appendCss(document, headerCssfragment);
        break;
      case 'moudle':
        const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
        const moudleCssfragment = `#${id}{${content}}`;
        this.#appendFragment(document, APP_ROOT_ID, moudleHtmlfragment);
        threepioLog('Processed moudle append CSS:', moudleCssfragment);
        this.#appendCss(document, moudleCssfragment);
        break;
      default:
        threepioWarn('Processed append key:', type, ' data:', fragment);
        break;
    }
  }

  /**
   * 
   * @param content  css content to be appended.
   * @description This method appends the provided CSS content to the document's head element.
   */
  #appendCss(document: Document, content: string): void {
    threepioLog('Processed append CSS:', content);
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
          threepioLog('Appended CSS to body in DOM (no head found).');
        } else {
          threepioWarn('Cannot find head or body to append CSS in DOM.');
        }
      }
    } catch (e) {
      threepioError('Error appending CSS to DOM:', e);
    }
  }

  #appendFragmentById(document: Document, selectId: string, content: string): void {
    this.#appendFragment(document, `#${selectId}`, content);
  }

  /**
   * 
   * @param selectId  The ID of the parent element to which the HTML content will be appended.
   * @param content  The HTML content to be appended.
   * @description This method appends the provided HTML content to the document's body element.
   */
  #appendFragment(document: Document, selectId: string, content: string): void {
    threepioLog('Process append HTML:', selectId, content);
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
          threepioLog('Created new parent element:', selectId, content);
        } else {
          const tempDiv = document.createElement('div');
          tempDiv.innerHTML = content;
          if (tempDiv.getAttribute('class')) {
            parentElement.setAttribute('class', tempDiv.getAttribute('class'));
            threepioLog('Set attribute:', tempDiv.getAttribute('class'));
          }
          if (tempDiv.getAttribute('style')) {
            parentElement.setAttribute('style', tempDiv.getAttribute('style'));
            threepioLog('Set attribute:', tempDiv.getAttribute('style'));
          }
        }
        appendHtml(content, parentElement);
      }
      threepioLog('Appended HTML to DOM.');
    } catch (e) {
      threepioError('Error appending HTML to DOM:', e);
    }
  }
}
