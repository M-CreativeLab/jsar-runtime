

import { Capability } from '../interface'; // Adjust the path as needed
import { Controller, createController } from './Controller';
import { PerformanceTracer } from '../../utils/PerformanceTracer';
import { Operater } from './Operater';
import { EmitterEventType } from './interfaces';

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
  #controller: Controller;
  #operater: Operater;
  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#controller = createController(new PerformanceTracer());
    this.#browsingContext = browsingContext;
    this.#operater = new Operater();
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
  async request(input: string): Promise<void> {
    this.#document = this.#browsingContext.start(htmlText, 'text/html', 'text');
    try {
      this.#controller.on(EmitterEventType.append, (data) => {
        console.log('Agent: Received append data:', data);
        this.#operater.operate(this.#document, data);
      });
      await this.#controller.generatePageStream(input);
      // const htmlcontext = `
      // <html>
      //   <head>${this.#document.head.innerHTML}</head>
      //   <body>${this.#document.body.innerHTML}</body>
      // </html>`;
      this.#operater.saveHtmlToFile(this.#document.documentElement.innerHTML, input);
    } catch (error) {
      console.error('Agent: Error creating task:', error);
    }
  }
}
