import { JSARDOM } from '@yodaos-jsar/dom';
import { extname } from 'node:path';

import { getPerformanceNow, isWebXRSupported } from '@transmute/env';
import { reportDocumentEvent } from '@transmute/messaging';
import { NativeDocumentOnTransmute } from './jsardom/TransmuteImpl';
import { ResourceLoaderOnTransmute } from './jsardom/ResourceLoader';

// viewers
import createModel3dViewer from './viewers/model3d';  // glb, gltf ...
import createImage2dViewer from './viewers/image2d';  // png, jpg, etc ...
import createSplineDesignViewer from './viewers/splinedesign';  // splinedesign
import { Threepio } from './threepio';

Object.defineProperty(BABYLON.PrecisionDate, 'Now', {
  get: () => getPerformanceNow(),
});

/**
 * Execute the XSML code or URL.
 * 
 * TODO: XSML will be deprecated.
 * 
 * @param gl the WebGL rendering context.
 * @param codeOrUrl the XSML code or URL.
 * @param urlBase the base URL.
 */
async function evaluateXSML(gl: WebGLRenderingContext | WebGL2RenderingContext, codeOrUrl: string, urlBase: string) {
  const nativeDocument = new NativeDocumentOnTransmute(gl);
  if (isWebXRSupported()) {
    nativeDocument.configureDefaultXrExperience(gl);
    await nativeDocument.enterDefaultXrExperience();
  } else {
    console.info(`Skip enabling WebXR experience, reason: WebXR is not enabled.`);
  }

  console.info(`loading a JSAR document`, codeOrUrl, { url: urlBase });
  let dom: JSARDOM<NativeDocumentOnTransmute> = null;

  try {
    dom = new JSARDOM(codeOrUrl, {
      url: urlBase,
      nativeDocument,
    });
    await dom.load();
    reportDocumentEvent(nativeDocument.id, 'loaded');

    const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
    {
      await dom.waitForSpaceReady();
      reportDocumentEvent(nativeDocument.id, 'DOMContentLoaded');
    }
    spaceNode.setEnabled(true);
    nativeDocument.dispatchDocumentLoadedEvent();
  } catch (err) {
    if (dom != null) {
      // Just unload and close the document
      await dom.unload();
      dom.nativeDocument.close();
    }
    // Report the error
    throw err;
  }
}

export class TransmuteRuntime2 extends EventTarget {
  #browsingContext: Transmute.BrowsingContext;
  #threepio: Threepio;
  constructor(private gl: WebGLRenderingContext | WebGL2RenderingContext, private id: number) {
    super();
    {
      /**
       * Print the supported WebGL extensions and versions.
       */
      const exts = gl.getSupportedExtensions();
      console.info(`[WebGL] supported extensions(${exts.length}):`);
      for (let extName of exts) {
        console.info(`  - ${extName}`);
      }
      console.info(`[JSARDOM] version=${JSARDOM.version}`);
    }
    {
      /**
       * Initialize the `BrowsingContext` instance.
       */
      const { BrowsingContext } = process._linkedBinding('transmute:dom');
      const browsingContext = new BrowsingContext();
      browsingContext.setResourceLoader(new ResourceLoaderOnTransmute());
      this.#browsingContext = browsingContext;
      this.#threepio = new Threepio(browsingContext);
    }
    this.dispatchEvent(new Event('rendererReady'));
  }

  async start(input: string) {
    console.info(`Content(#${this.id}): receiving a document request: ${input}`);
    // check codeOrUrl is a url or path
    if (input.startsWith('http:') || input.startsWith('https:') || input.startsWith('/')) {
      await this.load(input);
    } else {
      await this.#threepio.request(input);
    }
  }

  private async load(codeOrUrl: string, urlBase?: string) {
    if (!this.gl) {
      throw new TypeError('The webgl is not ready or lost context state');
    }

    // Override the `codeOrUrl` with the example url if the debug mode is enabled.
    if (process.env.JSAR_DEBUG_ENABLED === 'yes' && process.env.JSAR_EXAMPLE_URL) {
      codeOrUrl = process.env.JSAR_EXAMPLE_URL;
    }

    let urlObj: URL = null;
    let loadAsHTML = true; /** If load the content as HTML */

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
     * - [x] `xsml` for mixed reality content.
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
      case '.glb':
      case '.gltf':
        codeOrUrl = createModel3dViewer(codeOrUrl, { playAnimation: true });
        urlBase = urlObj.href;
        loadAsHTML = false;
        break;
      case '.png':
      case '.jpg':
      case '.jpeg':
        codeOrUrl = createImage2dViewer(codeOrUrl);
        urlBase = urlObj.href;
        loadAsHTML = false;
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
      case '.xsml':
        loadAsHTML = false;
        break;
      case '.splinecode':
        const sourceBlob = new Blob([createSplineDesignViewer(codeOrUrl)], { type: 'text/plain' });
        codeOrUrl = URL.createObjectURL(sourceBlob);
        break;
      default:
        break;
    }

    // Start the browsing context.
    if (loadAsHTML) {
      this.#browsingContext.start(codeOrUrl, 'text/html');
    } else {
      this.#browsingContext.start('', 'text/html');
    }

    // If not loading as HTML, evaluate the XSML.
    if (!loadAsHTML) {
      await evaluateXSML(this.gl, codeOrUrl, urlBase);
    }
    console.info(`Content(#${this.id}): the document is loaded successfully.`);
  }
}
