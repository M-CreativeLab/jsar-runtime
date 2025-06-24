import { Capability } from '../interface'; // Adjust the path as needed
import { RequestFlowManager } from './RequestFlowManager';
import { DomOperator } from './DomOperator';
import { EmitData } from './interfaces';
import { getThreepioApiProvider, getThreepioApiModelId, getThreepioApiEndpoint } from '@transmute/env';
import { reportThreepioError, reportThreepioInfo } from '../../utils/threepioLog';

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
  #operator: DomOperator;

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#manager = new RequestFlowManager();
    this.#browsingContext = browsingContext;
    this.#operator = new DomOperator();
  }

  /**
   * @param input input string that will be processed to generate HTML content.
   * @returns A Promise that resolves when the HTML content has been generated and displayed in the space.
   * and calls the manager to generate the HTML content based on the input string.
   * It then stops the performance tracker and reports the performance metrics.
   * Finally, it saves the generated HTML content to a file.
   */
  async request(input: string): Promise<void> {
    reportThreepioInfo('Agent: Starting request with input:', input,
      'provider', getThreepioApiProvider(),
      'modelid', getThreepioApiModelId(),
      'endpoint', getThreepioApiEndpoint());
    this.#document = this.#browsingContext.start(htmlText, 'text/html', 'source');
    try {
      this.#manager.on('append', (data: EmitData) => {
        reportThreepioInfo('Agent: Received append data:', data);
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
      reportThreepioInfo('Agent: Generated HTML content:', htmlcontext);
    } catch (error) {
      reportThreepioError('Agent: Error creating task:', error);
    }
  }
}
