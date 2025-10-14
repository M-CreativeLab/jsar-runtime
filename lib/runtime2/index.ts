import { extname } from 'node:path';

import { type DocumentRequestEvent } from '@transmute/messaging';
import { ResourceLoaderOnTransmute } from './ResourceLoader';

// viewers
import createModel3dViewer from './viewers/model3d';  // glb, gltf ...
import createImage2dViewer from './viewers/image2d';  // png, jpg, etc ...
import createSplineDesignViewer from './viewers/splinedesign';  // splinedesign
// import { Threepio } from './threepio';

export class TransmuteRuntime2 extends EventTarget {
  #resourceLoader: ResourceLoaderOnTransmute = new ResourceLoaderOnTransmute();
  #browsingContext: Transmute.BrowsingContext;
  // #threepio: Threepio;

  constructor(private id: number) {
    super();
    {
      /**
       * Initialize the `BrowsingContext` instance.
       */
      const { BrowsingContext } = process._linkedBinding('transmute:dom');
      const browsingContext = new BrowsingContext();
      browsingContext.setResourceLoader(this.#resourceLoader);
      this.#browsingContext = browsingContext;
      // this.#threepio = new Threepio(browsingContext);
    }
    this.dispatchEvent(new Event('rendererReady'));
  }

  async start(inputEvent: DocumentRequestEvent) {
    // Report the document event before loading.
    console.info(`Content(#${this.id}): receiving a document request: ${inputEvent.url}`);

    // Setup the default headers for the resource loader.
    this.#resourceLoader.setDefaultHeaders(inputEvent.defaultHTTPHeaders);

    // Handle the request URL.
    const requestUrl = inputEvent.url;
    if (requestUrl.startsWith('http:') ||
      requestUrl.startsWith('https:') ||
      requestUrl.startsWith('/') // for local path such as `/path/to/file.html`
    ) {
      await this.load(requestUrl);
    } else {
      // await this.#threepio.request(requestUrl);
    }
  }

  private async load(codeOrUrl: string, urlBase?: string) {
    // Override the `codeOrUrl` with the example url if the debug mode is enabled.
    if (process.env.JSAR_DEBUG_ENABLED === 'yes' && process.env.JSAR_EXAMPLE_URL) {
      codeOrUrl = process.env.JSAR_EXAMPLE_URL;
    }

    let urlObj: URL = null;
    let inputType: 'source' | 'url' = 'url';

    /**
     * If the input is a path, convert it to a URL.
     */
    if (codeOrUrl.startsWith('/')) {
      urlObj = new URL(codeOrUrl, 'file://');
      codeOrUrl = urlObj.href;
    }

    if (urlObj == null) {
      urlObj = new URL(codeOrUrl);
    }

    /**
     * Supports the formats to open directly:
     *
     * - [x] `html` for web page preview.
     * - [x] `glb`, `gltf`, `usdz`, etc, for 3D model preview.
     * - [x] `png`, `jpg`, etc, for image preview.
     * - [ ] `mp3`, `mp4`, `webm`, etc, for media preview.
     * - [ ] `pdf`, `epub`, etc, for document preview.
     *
     * BTW, the users could open the above formats in a html or xsml document in a new volume.
     */
    const urlExt = extname(urlObj.pathname);
    /**
     * TODO: implement this via mime type instead of the file extension?
     */
    switch (urlExt) {
      // TODO(yorkie): wait for `HTMLModelElement` supports the GLTF format.
      // case '.glb':
      // case '.gltf':
      case '.spz':
      case '.ksplat':
      case '.ply':
        codeOrUrl = createModel3dViewer(codeOrUrl, { playAnimation: true });
        inputType = 'source';
        urlBase = urlObj.href;
        console.info(`Using the following source code:`);
        console.info(codeOrUrl);
        break;
      case '.png':
      case '.jpg':
      case '.jpeg':
        codeOrUrl = createImage2dViewer(codeOrUrl);
        inputType = 'source';
        urlBase = urlObj.href;
        console.info(`Using the following source code:`);
        console.info(codeOrUrl);
        break;
      /**
       * TODO: support the following format viewers.
       */
      case '.mp3':
      case '.mp4':
      case '.webm':
      case '.pdf':
      case '.epub':
        throw new Error(`the format is not supported yet: ${urlExt}`);
      case '.splinecode':
        {
          const sourceBlob = new Blob([createSplineDesignViewer(codeOrUrl)], { type: 'text/plain' });
          codeOrUrl = URL.createObjectURL(sourceBlob);
        }
        break;
      default:
        break;
    }

    // Start the browsing context.
    const document = globalThis.document = this.#browsingContext.start(codeOrUrl, 'text/html', inputType, urlBase);
    console.info(`Content(#${this.id}): the document is loaded successfully.`, document);
  }
}
