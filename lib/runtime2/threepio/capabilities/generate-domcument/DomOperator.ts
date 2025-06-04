import { APP_ROOT_ID } from '.';
import { reportThreepioError, reportThreepioInfo, reportThreepioWarning } from '../../utils/threepioLog';
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
    if (document == null) {
      reportThreepioWarning('Document is null, cannot operate on it.');
      return;
    }
    const { type, fragment } = data;
    const { id, content, parentId } = fragment as any;
    switch (type) {
      case 'html':
        this.#appendFragment(document, parentId, content);
        break;
      case 'css':
        this.#appendCss(document, content);
        break;
      case 'header':
        if (!content) {
          reportThreepioWarning('Processed append key:', type, 'data:', fragment);
          break;
        }
        const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
        reportThreepioInfo('Processed  Header append CSS:', headerCssfragment);
        this.#appendCss(document, headerCssfragment);
        break;
      case 'moudle':
        const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
        const moudleCssfragment = `#${id}{${content}}`;
        this.#appendFragment(document, APP_ROOT_ID, moudleHtmlfragment);
        this.#appendCss(document, moudleCssfragment);
        reportThreepioInfo('Processed moudle append CSS:', moudleCssfragment);
        break;
      default:
        reportThreepioWarning('Processed append key:', type, ' data:', fragment);
        break;
    }
  }

  /**
   * @param content  css content to be appended.
   * @description This method appends the provided CSS content to the document's head element.
   */
  #appendCss(document: Document, content: string): void {
    reportThreepioInfo('Processed append CSS:', content);
    const styleElement = document.createElement('style');
    styleElement.appendChild(document.createTextNode(content));
    const head = document.head;
    if (head) {
      head.appendChild(styleElement);
    } else {
      const body = document.body || document.getElementsByTagName('body')[0];
      if (body) {
        body.appendChild(styleElement);
        reportThreepioInfo('Appended CSS to body in DOM (no head found).');
      } else {
        reportThreepioWarning('Cannot find head or body to append CSS in DOM.');
      }
    }
  }

  /**
   * @param selectId  The ID of the parent element to which the HTML content will be appended.
   * @param content  The HTML content to be appended.
   * @description This method appends the provided HTML content to the document's body element.
   */
  #appendFragment(document: Document, parentId: string, content: string): void {
    reportThreepioInfo('Process append HTML:', parentId, content);
    try {
      if (parentId === null) {
        appendHtml(content, document.body);
      } else {
        let parentElement = document.getElementById(parentId);
        if (!parentElement) {
          const newParentElement = parentElement = document.createElement('div');
          newParentElement.id = parentId;
          const bodyElement = document.body;
          if (bodyElement) {
            bodyElement.appendChild(newParentElement);
          }
          reportThreepioInfo('Created new parent element:', parentId, content);
        } else {
          const tempDiv = document.createElement('div');
          tempDiv.innerHTML = content;
          if (tempDiv.getAttribute('class')) {
            parentElement.setAttribute('class', tempDiv.getAttribute('class'));
            reportThreepioInfo('Set attribute:', tempDiv.getAttribute('class'));
          }
          if (tempDiv.getAttribute('style')) {
            parentElement.setAttribute('style', tempDiv.getAttribute('style'));
            reportThreepioInfo('Set attribute:', tempDiv.getAttribute('style'));
          }
        }
        appendHtml(content, parentElement);
      }
      reportThreepioInfo('Appended HTML to DOM.');
    } catch (e) {
      reportThreepioError('Error appending HTML to DOM:', e);
    }
  }
}
