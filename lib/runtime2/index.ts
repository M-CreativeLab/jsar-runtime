import { JSARDOM } from '@yodaos-jsar/dom';
import { extname } from 'node:path';

import { getPerformanceNow, isWebXRSupported } from '@transmute/env';
import { reportDocumentEvent } from '@transmute/messaging';
import { NativeDocumentOnTransmute } from './jsardom/TransmuteImpl';

// viewers
import createModel3dViewer from './viewers/model3d';  // glb, gltf ...
import createImage2dViewer from './viewers/image2d';  // png, jpg, etc ...

Object.defineProperty(BABYLON.PrecisionDate, 'Now', {
  get: () => getPerformanceNow(),
});

export class TransmuteRuntime2 extends EventTarget {
  #nativeDocument: NativeDocumentOnTransmute;

  constructor(private gl: WebGLRenderingContext | WebGL2RenderingContext, private id: number) {
    super();
    this.dispatchEvent(new Event('rendererReady'));
  }

  async start(url: string) {
    console.info(`Content(#${this.id}): receiving a document request: ${url}`);
    if (!this.#nativeDocument) {
      throw new TypeError('Call prepare() before start()');
    }
    await this.load(url, this.#nativeDocument);
  }

  onGpuBusy() {
    // TODO
  }

  async prepare() {
    const exts = this.gl.getSupportedExtensions();
    console.info(`[WebGL] supported extensions(${exts.length}):`);
    for (let extName of exts) {
      console.info(`  - ${extName}`);
    }
    console.info(`[JSARDOM] version=${JSARDOM.version}`);

    this.#nativeDocument = new NativeDocumentOnTransmute(this.gl);
    if (isWebXRSupported()) {
      await this.#nativeDocument.enterDefaultXrExperience();
    } else {
      console.info(`Skip enabling WebXR experience, reason: WebXR is not enabled.`);
    }
    console.info(`The runtime#${this.id} has been ready.`);
  }

  private async load(
    codeOrUrl: string,
    nativeDocument: NativeDocumentOnTransmute,
    urlBase?: string
  ) {
    if (!this.gl) {
      throw new TypeError('The webgl is not ready or lost context state');
    }

    // Override the `codeOrUrl` with the example url if the debug mode is enabled.
    if (process.env.JSAR_DEBUG_ENABLED === 'yes' && process.env.JSAR_EXAMPLE_URL) {
      codeOrUrl = process.env.JSAR_EXAMPLE_URL;
    }

    try {
      const urlObj = new URL(codeOrUrl);
      /**
       * Supports the formats to open directly:
       * 
       * - [x] `xsml` for mixed reality content.
       * - [ ] `html` for web page preview.
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
          console.info(`switched to the 3d model viewer.`);
          break;
        case '.png':
        case '.jpg':
        case '.jpeg':
          codeOrUrl = createImage2dViewer(codeOrUrl);
          urlBase = urlObj.href;
          console.info(`switched to the 2d image viewer.`);
          break;
        case '.html':
        case '.mp3':
        case '.mp4':
        case '.webm':
        case '.pdf':
        case '.epub':
          throw new Error(`the format is not supported yet: ${urlExt}`);
        default:
          break;
      }
    } catch (_err) {
      urlBase = 'https://example.com/'
    }

    console.info(`loading a JSAR document`, codeOrUrl);
    const dom = new JSARDOM(codeOrUrl, {
      url: urlBase,
      nativeDocument,
    });

    try {
      await dom.load();
      reportDocumentEvent(nativeDocument.id, 'loaded');

      const spaceNode = dom.document.space.asNativeType<BABYLON.TransformNode>();
      {
        await dom.waitForSpaceReady();
        reportDocumentEvent(nativeDocument.id, 'DOMContentLoaded');
      }
      spaceNode.setEnabled(true);
    } catch (err) {
      // Just unload and close the document
      await dom.unload();
      dom.nativeDocument.close();
      // Report the error
      throw err;
    }
  }
}
