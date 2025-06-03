import { Capability } from '../interface'; // Adjust the path as needed
import { RequestFlowManager } from './RequestFlowManager';
import { Operator } from './Operator';
import { EmitterEventType } from './interfaces';
import { getApiModelId, getApiProvider, getApiEndpoint } from '@transmute/env';

export const APP_ROOT_ID = 'app-root';

const htmlText = `
<html>
  <head></head>
  <body style="display:flex; justify-content:center; align-items:center;">
    <div id=\"${APP_ROOT_ID}\">
    </div>
  </body>
</html>`;

export class GenerateDocumentCapability implements Capability {
  #browsingContext: Transmute.BrowsingContext;
  #document: Document;
  #manager: RequestFlowManager;
  #operator: Operator;
  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#manager = new RequestFlowManager();
    this.#browsingContext = browsingContext;
    this.#operator = new Operator();
  }

  /**
   * 
   * @param input input string that will be processed to generate HTML content.
   * @returns A Promise that resolves when the HTML content has been generated and displayed in the space.
   * @description This method initializes the HTML document, starts the performance tracker,
   * and calls the manager to generate the HTML content based on the input string.
   * It then stops the performance tracker and reports the performance metrics.
   * Finally, it saves the generated HTML content to a file.
   */
  async request(input: string): Promise<void> {
    console.log('Agent: Starting request with input:', input,
      'provider', getApiProvider(),
      'modelid', getApiModelId(),
      'endpoint', getApiEndpoint());
    this.#document = this.#browsingContext.start(htmlText, 'text/html', 'text');
    try {
      this.#manager.on(EmitterEventType.append, (data) => {
        console.log('Agent: Received append data:', data);
        this.#operator.operate(this.#document, data);
      });
      await this.#manager.executeFlow(input);
      const htmlcontext = `
      <html>
        <head>${this.#document.head.innerHTML}</head>
        <body style="display:flex; justify-content:center; align-items:center;">
          <div id=\"${APP_ROOT_ID}\">
          ${this.#document.body.innerHTML}
          </div>
        </body>
      </html>
      `;
      console.log('Agent: Generated HTML content:', htmlcontext);
    } catch (error) {
      console.error('Agent: Error creating task:', error);
    }
  }
}
