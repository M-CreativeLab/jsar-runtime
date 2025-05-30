import { Capability } from './capabilities/interface';
import { GenerateDocumentCapability } from './capabilities/generate-domcument';

export class Threepio {

  #capabilities: Capability[] = [];

  constructor(browsingContext: Transmute.BrowsingContext) {
    this.#capabilities.push(new GenerateDocumentCapability(browsingContext));
  }

  public async request(input: string): Promise<void> {
    const defaultCapability = this.#capabilities[0];
    if (defaultCapability) {
      await defaultCapability.request(input);
    }
  }
}
