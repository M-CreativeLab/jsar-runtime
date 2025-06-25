import { APP_ROOT_ID } from '.';
import { wrapTaskFlowMonitor } from '../../trace/wrapTaskFlowMonitor';
import { reportThreepioError, reportThreepioInfo, reportThreepioWarning } from '../../utils/threepioLog';
import { EmitData, HtmlFragment } from './interfaces';

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
    const appendCss = wrapTaskFlowMonitor(this.appendCss.bind(this), { type: 'DOMOperation' });
    const appendFragment = wrapTaskFlowMonitor(this.appendFragment.bind(this), { type: 'DOMOperation' });
    const { type, fragment } = data;
    const { id, content } = fragment as any;
    switch (type) {
      case 'html':
        appendFragment(data, document);
        break;
      case 'css':
        appendCss(data, document);
        break;
      case 'header':
        if (!content) {
          reportThreepioWarning('Processed append key:', type, 'data:', fragment);
          break;
        }
        const headerCssfragment = `#${APP_ROOT_ID}{${content}}`;
        reportThreepioInfo('Processed  Header append CSS:', headerCssfragment);
        data.fragment = { content: headerCssfragment, parentId: APP_ROOT_ID, ...data.fragment } as HtmlFragment;
        appendCss(data, document);
        break;
      case 'moudle':
        const htmlFragment = fragment as HtmlFragment;
        const moudleHtmlfragment = `<div id=\'${id}\'></div>`;
        const moudleCssfragment = `#${id}{${content}}`;
        htmlFragment.parentId = APP_ROOT_ID;
        data.fragment.content = moudleHtmlfragment;
        appendFragment(data, document);
        data.fragment.content = moudleCssfragment;
        appendCss(data, document);
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
  protected appendCss(data: EmitData, document: Document): void {
    const { fragment } = data;
    const { content } = fragment;
    if (document == null) {
      reportThreepioWarning('Document is null, cannot operate on it.');
      return;
    }
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
  protected appendFragment(data: EmitData, document: Document): void {
    const { fragment } = data;
    const { content, parentId } = fragment as any;
    if (document == null) {
      reportThreepioWarning('Document is null, cannot operate on it.');
      return;
    } try {
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
